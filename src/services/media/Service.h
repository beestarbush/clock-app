#ifndef SERVICES_MEDIA_SERVICE_H
#define SERVICES_MEDIA_SERVICE_H

#include "Model.h"
#include <QDateTime>
#include <QMetaObject>
#include <QObject>
#include <QStringList>
#include <QTimer>

namespace Services::WebSocket
{
class Service;
}
namespace Services::Rest
{
class Service;
}

namespace Services::Media
{
class Service : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Media::Model* model READ model CONSTANT)
    Q_PROPERTY(bool syncing READ syncing NOTIFY syncingChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(bool startupCheckInProgress READ startupCheckInProgress NOTIFY startupCheckInProgressChanged)

  public:
    explicit Service(Services::WebSocket::Service& webSocket, Services::Rest::Service& rest, QObject* parent = nullptr);

    Model* model();
    bool syncing() const;
    QString lastError() const;
    bool startupCheckInProgress() const;

    Q_INVOKABLE QString getMediaPath(const QString& name) const;

  signals:
    void syncingChanged();
    void lastErrorChanged();
    void syncCompleted();
    void startupCheckInProgressChanged();

  private:
    bool isValidFile(const QString& filePath) const;
    QString getMediaDirectory() const;

    void syncWithServerFiles(const QStringList& serverFilenames);
    void downloadMedia(const QString& mediaId);
    void completeSyncWithSuccess();
    void completeSyncWithError(const QString& error);

    void setSyncing(bool syncing);
    void performStartupCheck();
    void completeStartupCheck();
    void setStartupCheckInProgress(bool inProgress);

    void onMediaReceived(const QJsonObject& data);

    Model m_model;
    QTimer m_startupTimeoutTimer;
    Services::WebSocket::Service& m_webSocket;
    Services::Rest::Service& m_rest; // Kept for binary file downloads only

    bool m_syncing;
    bool m_startupCheckInProgress;
    QMetaObject::Connection m_startupConnectionWatcher;
    QString m_lastError;
    QStringList m_pendingDownloads;
};
} // namespace Services::Media

#endif // SERVICES_MEDIA_SERVICE_H
