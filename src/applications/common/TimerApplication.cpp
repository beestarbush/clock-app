#include "TimerApplication.h"
#include "services/media/Service.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(TimerApplicationLog, "TimerApplication")

using namespace Common;

TimerApplication::TimerApplication(const QString& id,
                                   Type type,
                                   const QString& displayName,
                                   int order,
                                   Watchface watchface,
                                   Services::Media::Service& media,
                                   QObject* parent)
    : Application(id, type, displayName, order, watchface, parent),
      m_configuration(new TimerConfiguration(id, parent)),
      m_media(media),
      m_timer(this)
{
    // Refresh background when media sync completes
    connect(&m_media, &Services::Media::Service::syncCompleted, this, [this]() {
        emit m_configuration->backgroundChanged();
    });

    // Recalculate time when target timestamp changes
    connect(m_configuration, &TimerConfiguration::timestampChanged, this, [this]() {
        calculateTime();
    });
}

Common::TimerConfiguration* TimerApplication::configuration() const
{
    return m_configuration;
}

void TimerApplication::applyConfiguration(const Common::TimerConfiguration& configuration)
{
    *m_configuration = configuration;
    qCDebug(TimerApplicationLog) << metaObject()->className() << "configuration:" << *m_configuration;
}

void TimerApplication::applyConfiguration(const QJsonObject& configuration)
{
    m_configuration->fromJson(configuration);
    qCDebug(TimerApplicationLog) << metaObject()->className() << "configuration:" << *m_configuration;
}

void TimerApplication::startTimer()
{
    calculateTime();

    if (!m_timer.isActive()) {
        connect(&m_timer, &QTimer::timeout, this, [this]() {
            calculateTime();
        });
        m_timer.start(1000); // Update every second
    }
}

void TimerApplication::stopTimer()
{
    if (m_timer.isActive()) {
        disconnect(&m_timer, &QTimer::timeout, this, nullptr);
        m_timer.stop();
    }
}

quint64 TimerApplication::years() const
{
    return m_years;
}

quint64 TimerApplication::days() const
{
    return m_days;
}

quint64 TimerApplication::daysInWeek() const
{
    return m_daysInWeek;
}

quint64 TimerApplication::weeks() const
{
    return m_weeks;
}

quint64 TimerApplication::hours() const
{
    return m_hours;
}

quint64 TimerApplication::minutes() const
{
    return m_minutes;
}

quint64 TimerApplication::seconds() const
{
    return m_seconds;
}

void TimerApplication::setYears(quint64 years)
{
    if (m_years != years) {
        m_years = years;
        emit yearsChanged(m_years);
    }
}

void TimerApplication::setDays(quint64 days)
{
    if (m_days != days) {
        m_days = days;
        emit daysChanged(m_days);
    }
}

void TimerApplication::setDaysInWeek(quint64 daysInWeek)
{
    if (m_daysInWeek != daysInWeek) {
        m_daysInWeek = daysInWeek;
        emit daysInWeekChanged(m_daysInWeek);
    }
}

void TimerApplication::setWeeks(quint64 weeks)
{
    if (m_weeks != weeks) {
        m_weeks = weeks;
        emit weeksChanged(m_weeks);
    }
}

void TimerApplication::setHours(quint64 hours)
{
    if (m_hours != hours) {
        m_hours = hours;
        emit hoursChanged(m_hours);
    }
}

void TimerApplication::setMinutes(quint64 minutes)
{
    if (m_minutes != minutes) {
        m_minutes = minutes;
        emit minutesChanged(m_minutes);
    }
}

void TimerApplication::setSeconds(quint64 seconds)
{
    if (m_seconds != seconds) {
        m_seconds = seconds;
        emit secondsChanged(m_seconds);
    }
}
