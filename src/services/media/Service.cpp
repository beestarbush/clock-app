#include "Service.h"
#include "services/rest/Service.h"
#include "services/websocket/Service.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>

using namespace Services::Media;

#ifdef PLATFORM_IS_TARGET
const QString MEDIA_PATH = QStringLiteral("/usr/share/bee/media");
#else
const QString MEDIA_PATH = QStringLiteral("/workdir/build/bee/media");
#endif
const QString DEFAULT_MEDIA = QStringLiteral("qrc:/media/default.gif");
constexpr int MIN_MEDIA_FILE_SIZE = 50;         // Minimum reasonable file size in bytes
constexpr int INITIAL_SYNC_DELAY_MS = 5000;     // 5 seconds

Service::Service(Services::WebSocket::Service& webSocket, Services::Rest::Service& rest, QObject* parent)
    : QObject(parent),
      m_model(this),
      m_startupTimeoutTimer(this),
      m_webSocket(webSocket),
      m_rest(rest),
      m_syncing(false),
      m_startupCheckInProgress(false)
{
    // Subscribe to media change notifications from backend
    m_webSocket.subscribe(Services::WebSocket::Topic::Media);
    connect(&m_webSocket, &Services::WebSocket::Service::publishReceived, this, [this](const Services::WebSocket::Topic& topic, const QJsonObject& data) {
        if (topic == Services::WebSocket::Topic::Media) {
            onMediaReceived(data);
        }
    });
    connect(&m_webSocket, &Services::WebSocket::Service::connectedChanged, this, [this]() {
        if (m_webSocket.connected()) {
             m_webSocket.request(Services::WebSocket::Method::GetMedia, QJsonObject(), [this](bool success, const QJsonObject& result, const QString& error) {
                 if (success) {
                     onMediaReceived(result);
                 } else {
                     qWarning() << "Failed to get media:" << error;
                 }
             });
        }
    });

    performStartupCheck();
}

Model* Service::model()
{
    return &m_model;
}

bool Service::syncing() const
{
    return m_syncing;
}

QString Service::lastError() const
{
    return m_lastError;
}

bool Service::startupCheckInProgress() const
{
    return m_startupCheckInProgress;
}

QString Service::getMediaPath(const QString& name) const
{
    if (name.isEmpty()) {
        return QString();
    }

    QString mediaDir = getMediaDirectory();
    QString fullPath = QDir(mediaDir).absoluteFilePath(name);

    // Verify the file exists and is valid
    if (QFileInfo::exists(fullPath) && isValidFile(fullPath)) {
        return fullPath;
    }

    qWarning() << "Requested media not found or invalid:" << fullPath << ", falling back to default.";
    return DEFAULT_MEDIA;
}

bool Service::isValidFile(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);

    // Check file extension
    QString suffix = fileInfo.suffix().toLower();
    if (suffix != "gif" && suffix != "png" && suffix != "jpg" && suffix != "jpeg") {
        qDebug() << "Invalid media file extension:" << filePath;
        return false;
    }

    // Basic file size check (avoid empty files)
    if (fileInfo.size() < MIN_MEDIA_FILE_SIZE) {
        qDebug() << "Invalid media file size:" << filePath;
        return false;
    }

    return true;
}

QString Service::getMediaDirectory() const
{
    return MEDIA_PATH;
}

void Service::onMediaReceived(const QJsonObject& data)
{
    qInfo() << "Received media notification from backend";
    if (data.isEmpty()) {
        qWarning() << "Received empty media data";
         return;
    }

    setSyncing(true);

    QJsonArray filesArray = data["files"].toArray();
    QStringList serverFilenames;
    for (const auto& val : filesArray) {
        serverFilenames.append(val.toString());
    }

    syncWithServerFiles(serverFilenames);
}

void Service::syncWithServerFiles(const QStringList& serverFilenames)
{
    // Current local files
    QString mediaDir = getMediaDirectory();
    QDir dir(mediaDir);
    if (!dir.exists()) {
        dir.mkpath(mediaDir);
    }
    QStringList localFilenames = dir.entryList(QDir::Files);

    QStringList mediaToDownload;
    QStringList mediaToDelete;

    for (const QString& serverFile : serverFilenames) {
        if (!localFilenames.contains(serverFile)) {
            mediaToDownload.append(serverFile);
        }
    }

    for (const QString& localFile : localFilenames) {
        if (!serverFilenames.contains(localFile)) {
            if (isValidFile(mediaDir + "/" + localFile)) {
                mediaToDelete.append(localFile);
            }
        }
    }

    // Delete removed files and update model
    for (const QString& file : mediaToDelete) {
        QFile::remove(mediaDir + "/" + file);
        m_model.removeItem(file);
    }

    // Load already-present local files into the model
    for (const QString& file : serverFilenames) {
        if (!mediaToDownload.contains(file)) {
            QString filePath = mediaDir + "/" + file;
            QFileInfo fileInfo(filePath);
            QString suffix = fileInfo.suffix().toLower();
            QString type = "unknown";
            if (suffix == "gif") type = "image/gif";
            else if (suffix == "png") type = "image/png";
            else if (suffix == "jpg" || suffix == "jpeg") type = "image/jpeg";

            if (!m_model.contains(file)) {
                m_model.addItem(new Item(file, file, filePath, type, fileInfo.size(), nullptr));
            }
        }
    }

    if (mediaToDownload.isEmpty()) {
        completeSyncWithSuccess();
        return;
    }

    // Download new files
    m_pendingDownloads = mediaToDownload;
    for (const QString& file : mediaToDownload) {
        downloadMedia(file);
    }
}

void Service::downloadMedia(const QString& filename)
{
    // Binary file download still uses REST
    QString endpoint = "media/" + filename;

    m_rest.download(endpoint, [this, filename](bool success, const QByteArray& data, const QString& error) {
        m_pendingDownloads.removeOne(filename);

        if (!success) {
            qWarning() << "Failed to download" << filename << ":" << error;
            if (m_pendingDownloads.isEmpty()) {
                if (m_lastError.isEmpty()) completeSyncWithSuccess();
                else completeSyncWithError("Some downloads failed");
            }
            return;
        }

        QString filePath = getMediaDirectory() + "/" + filename;
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(data);
            file.close();
            qDebug() << "Downloaded:" << filename;

            // Construct and add Item directly to the model
            QFileInfo fileInfo(filePath);
            QString suffix = fileInfo.suffix().toLower();
            QString type = "unknown";
            if (suffix == "gif") type = "image/gif";
            else if (suffix == "png") type = "image/png";
            else if (suffix == "jpg" || suffix == "jpeg") type = "image/jpeg";

            m_model.addItem(new Item(filename, filename, filePath, type, fileInfo.size(), nullptr));
        } else {
            qWarning() << "Failed to write downloaded file:" << filePath;
        }

        if (m_pendingDownloads.isEmpty()) {
            completeSyncWithSuccess();
        }
    });
}

void Service::completeSyncWithSuccess()
{
    setSyncing(false);
    m_lastError.clear();

    emit lastErrorChanged();
    emit syncCompleted();

    if (m_startupCheckInProgress) {
        completeStartupCheck();
    }

    qDebug() << "Media sync completed successfully";
}

void Service::completeSyncWithError(const QString& error)
{
    setSyncing(false);
    m_lastError = error;

    emit lastErrorChanged();

    qWarning() << "Media sync failed:" << error;

    if (m_startupCheckInProgress) {
        completeStartupCheck();
    }
}

void Service::performStartupCheck()
{
    if (startupCheckInProgress()) {
        return;
    }

    setSyncing(true);
    setStartupCheckInProgress(true);

    m_startupTimeoutTimer.setSingleShot(true);
    m_startupTimeoutTimer.setInterval(INITIAL_SYNC_DELAY_MS);
    connect(&m_startupTimeoutTimer, &QTimer::timeout, this, [this]() {
        if (m_startupCheckInProgress) {
            qWarning() << "Media startup check timed out, marking complete";
            completeStartupCheck();
        }
    });
    m_startupTimeoutTimer.start();

    // If already connected, sync immediately
    if (m_webSocket.connected()) {
        m_webSocket.request(Services::WebSocket::Method::GetMedia, QJsonObject(), [this](bool success, const QJsonObject& result, const QString& error) {
            if (!success) {
                completeSyncWithError("Failed to fetch media list: " + error);
                return;
            }

            onMediaReceived(result);
        });
        return;
    }

    // Otherwise wait for the connection
    qInfo() << "Media: Waiting for WebSocket connection (max" << INITIAL_SYNC_DELAY_MS << "ms)...";
    
     m_startupConnectionWatcher = connect(&m_webSocket,
                                         &Services::WebSocket::Service::connectedChanged,
                                         this,
                                         [this]() {
                                             if (m_webSocket.connected() && startupCheckInProgress()) {
                                                 disconnect(m_startupConnectionWatcher);
                                                 m_webSocket.request(Services::WebSocket::Method::GetMedia, QJsonObject(), [this](bool success, const QJsonObject& result, const QString& error) {
                                                     if (success) {
                                                         onMediaReceived(result);
                                                     } else {
                                                         completeSyncWithError("Failed to fetch media list: " + error);
                                                     }
                                                 });
                                             }
                                         });
}

void Service::completeStartupCheck()
{
    m_startupTimeoutTimer.stop();
    disconnect(m_startupConnectionWatcher);
    setStartupCheckInProgress(false);
    qInfo() << "Media startup check complete.";
}

void Service::setSyncing(bool syncing)
{
    if (m_syncing != syncing) {
        m_syncing = syncing;
        emit syncingChanged();
    }
}

void Service::setStartupCheckInProgress(bool inProgress)
{
    if (m_startupCheckInProgress != inProgress) {
        m_startupCheckInProgress = inProgress;
        emit startupCheckInProgressChanged();
    }
}
