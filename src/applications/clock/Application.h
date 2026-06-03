#ifndef APPS_CLOCK_H
#define APPS_CLOCK_H

#include "Configuration.h"
#include "applications/common/Application.h"
#include "websocket/Types.h"
#include <QObject>
#include <QTimer>

namespace Services::Media
{
class Service;
}

namespace Services::Audio
{
class Service;
}

namespace Applications::Clock
{
class Configuration;

class Application : public Common::Application
{
    Q_OBJECT
    Q_PROPERTY(Configuration* configuration READ configuration CONSTANT)

  public:
    Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media, Services::Audio::Service& audio, QObject* parent = nullptr);

    Configuration* configuration() const override;

    void applyConfiguration(const Configuration& configuration);
    void applyConfiguration(const QJsonObject& configuration);

  signals:
    void ticked(bool tickPhase);

  private:
    void onTimerTick();

    Configuration* m_configuration;
    Services::Media::Service& m_media;
    Services::Audio::Service& m_audio;
    QTimer m_timer;
    bool m_tickPhase; // true = tick, false = tack
};
} // namespace Applications::Clock

#endif // APPS_CLOCK_H