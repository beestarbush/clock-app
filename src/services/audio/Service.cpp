#include "Service.h"
#include "services/websocket/Service.h"

#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(AudioService, "AudioService")

using namespace Services::Audio;

Service::Service(Services::WebSocket::Service& webSocket, QObject* parent)
    : QObject(parent),
      m_webSocket(webSocket)
{
}

void Service::play(const QString& filename, Services::WebSocket::PlayMode mode)
{
    QJsonObject params;
    params["filename"] = filename;
    params["mode"] = playModeToString(mode);

    m_webSocket.request(Services::WebSocket::Method::PlaySound,
                        params,
                        [this, filename](bool success, const QJsonObject& result, const QString& error) {
                            onPlayResponse(success, result, error, filename);
                        });
}

void Service::stop()
{
    m_webSocket.request(Services::WebSocket::Method::StopSound,
                        QJsonObject{},
                        [this](bool success, const QJsonObject& result, const QString& error) {
                            onStopResponse(success, result, error);
                        });
}

QString Service::lastError() const
{
    return m_lastError;
}

void Service::setLastError(const QString& error)
{
    if (m_lastError != error) {
        m_lastError = error;
        emit lastErrorChanged();
    }
}

void Service::onPlayResponse(bool success, const QJsonObject& result, const QString& error, const QString& filename)
{
    Q_UNUSED(result);

    if (success) {
        setLastError(QString());
        emit playbackStarted(filename);
    }
    else {
        if (error == QStringLiteral("Disconnected")) {
            qCDebug(AudioService) << "Audio request dropped due to disconnect, ignoring:" << filename;
            return;
        }
        qCWarning(AudioService) << "Failed to play audio" << filename << ":" << error;
        setLastError(error);
        emit playbackFailed(filename, error);
    }
}

void Service::onStopResponse(bool success, const QJsonObject& result, const QString& error)
{
    Q_UNUSED(result);

    if (success) {
        setLastError(QString());
    }
    else {
        qCWarning(AudioService) << "Failed to stop audio:" << error;
        setLastError(error);
    }
}
