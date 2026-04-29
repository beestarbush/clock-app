#ifndef SERVICES_AUDIO_SERVICE_H
#define SERVICES_AUDIO_SERVICE_H

#include "services/websocket/Types.h"
#include <QObject>

namespace Services::WebSocket
{
class Service;
}

namespace Services::Audio
{
class Service : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)

  public:
    explicit Service(Services::WebSocket::Service& webSocket, QObject* parent = nullptr);

    void play(const QString& filename, Services::WebSocket::PlayMode mode = Services::WebSocket::PlayMode::Concurrent);
    void stop();

    bool playing() const;
    QString lastError() const;

  signals:
    void playingChanged();
    void lastErrorChanged();
    void playbackFailed(const QString& filename, const QString& error);
    void playbackStarted(const QString& filename);

  private:
    void onPlayResponse(bool success, const QJsonObject& result, const QString& error, const QString& filename);
    void onStopResponse(bool success, const QJsonObject& result, const QString& error);

    void setPlaying(bool playing);
    void setLastError(const QString& error);

    Services::WebSocket::Service& m_webSocket;
    bool m_playing;
    QString m_lastError;
};
} // namespace Services::Audio

#endif // SERVICES_AUDIO_SERVICE_H
