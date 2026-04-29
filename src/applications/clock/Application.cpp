#include "Application.h"
#include "services/audio/Service.h"
#include "services/media/Service.h"
#include <QDateTime>
#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(ClockApplication, "ClockApplication")

using namespace Applications::Clock;

Application::Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media, Services::Audio::Service& audio, QObject* parent)
    : Common::Application(id, type, displayName, order, watchface, parent),
      m_configuration(new Configuration(id, parent)),
      m_media(media),
      m_audio(audio),
      m_timer(this)
{
    // Refresh background when media sync completes
    connect(&m_media, &Services::Media::Service::syncCompleted, this, [this]() {
        emit m_configuration->backgroundChanged();
    });

    connect(&m_timer, &QTimer::timeout, this, &Application::onTimerTick);
    m_timer.start(1000);
}

void Application::onTimerTick()
{
    QDateTime now = QDateTime::currentDateTime();
    int second = now.time().second();
    int minute = now.time().minute();

    QString tickSound = m_configuration->tickSoundFile();
    if (!tickSound.isEmpty()) {
        m_audio.play(tickSound, Services::WebSocket::PlayMode::Concurrent);
    }

    if (second == 0 && minute == 0) {
        QString chimeSound = m_configuration->hourlyChimeSoundFile();
        if (!chimeSound.isEmpty()) {
            m_audio.play(chimeSound, Services::WebSocket::PlayMode::Concurrent);
        }
    }
}

Configuration* Application::configuration() const
{
    return m_configuration;
}

void Application::applyConfiguration(const Configuration& configuration)
{
    *m_configuration = configuration;
    qCDebug(ClockApplication) << "Configuration: " << *m_configuration;
}

void Application::applyConfiguration(const QJsonObject& configuration)
{
    m_configuration->fromJson(configuration);
    qCDebug(ClockApplication) << "Configuration: " << *m_configuration;
}
