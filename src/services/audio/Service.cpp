#include "Service.h"
#include "services/websocket/Service.h"

#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(AudioService, "AudioService")

using namespace Services::Audio;

Service::Service(Services::WebSocket::Service& webSocket, QObject* parent)
    : QObject(parent),
      m_webSocket(webSocket),
      m_playing(false)
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

bool Service::playing() const
{
    return m_playing;
}

QString Service::lastError() const
{
    return m_lastError;
}

void Service::setPlaying(bool playing)
{
    if (m_playing != playing) {
        m_playing = playing;
        emit playingChanged();
    }
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
        qCDebug(AudioService) << "Audio played:" << filename;
        setPlaying(false);
        setLastError(QString());
        emit playbackStarted(filename);
    }
    else {
        // Suppress "Disconnected" errors — these are transient (socket dropped mid-request)
        // and not meaningful to the user. Real backend errors will have a descriptive message.
        if (error == QStringLiteral("Disconnected")) {
            qCDebug(AudioService) << "Audio request dropped due to disconnect, ignoring:" << filename;
            return;
        }
        qCWarning(AudioService) << "Failed to play audio" << filename << ":" << error;
        setPlaying(false);
        setLastError(error);
        emit playbackFailed(filename, error);
    }
}

void Service::onStopResponse(bool success, const QJsonObject& result, const QString& error)
{
    Q_UNUSED(result);

    if (success) {
        qCDebug(AudioService) << "Audio playback stopped";
        setPlaying(false);
        setLastError(QString());
    }
    else {
        qCWarning(AudioService) << "Failed to stop audio:" << error;
        setLastError(error);
    }
}
