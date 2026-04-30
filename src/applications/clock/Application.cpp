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
      m_timer(this),
      m_tickPhase(true)
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
    emit ticked();

    QDateTime now = QDateTime::currentDateTime();
    int second = now.time().second();
    int minute = now.time().minute();

    // Alternate between tick and tack sound each second
    QString sound = m_tickPhase ? m_configuration->tickSoundFile() : m_configuration->tackSoundFile();
    if (!sound.isEmpty()) {
        m_audio.play(sound, Services::WebSocket::PlayMode::Concurrent);
    }
    m_tickPhase = !m_tickPhase;

    if (second == 0 && minute == 0) {
        QString chimeSound = m_configuration->hourlyChimeSoundFile();
        if (!chimeSound.isEmpty()) {
            // Convert to 12-hour format, treating 0 as 12
            int hour = now.time().hour() % 12;
            hour = hour == 0 ? 12 : hour;

            for (int i = 0; i < hour; i++) {
                m_audio.play(chimeSound, Services::WebSocket::PlayMode::Queue);
            }
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
