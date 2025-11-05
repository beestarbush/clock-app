#include "Application.h"
#include "services/media/Service.h"
#include <QDateTime>
#include <QDebug>
using namespace Applications::Countdown;

constexpr quint64 SECONDS_IN_MINUTE = 60;
constexpr quint64 SECONDS_IN_HOUR = SECONDS_IN_MINUTE * 60;
constexpr quint64 SECONDS_IN_A_DAY = SECONDS_IN_HOUR * 24;
constexpr quint64 DAYS_IN_A_WEEK = 7;
constexpr quint64 DAYS_IN_YEAR = 365;

Application::Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media, QObject* parent)
    : Common::Application(id, type, displayName, order, watchface, parent),
      m_configuration(new Common::TimerConfiguration(id, parent)),
      m_media(media),
      m_years(0),
      m_days(0),
      m_daysInWeek(0),
      m_weeks(0),
      m_hours(0),
      m_minutes(0),
      m_seconds(0),
      m_timer(this),
      m_finished(false)
{
    startTimer();

    // Refresh background when media sync completes
    connect(&m_media, &Services::Media::Service::syncCompleted, this, [this]() {
        emit m_configuration->backgroundChanged();
    });

    // Recalculate time when target timestamp changes
    connect(m_configuration, &Common::TimerConfiguration::timestampChanged, this, &Application::calculateTimeRemaining);
}

Common::TimerConfiguration* Application::configuration() const
{
    return m_configuration;
}

void Application::applyConfiguration(const Common::TimerConfiguration& configuration)
{
    *m_configuration = configuration;
    qDebug() << "Countdown configuration: " << *m_configuration;
}

void Application::applyConfiguration(const QJsonObject& configuration)
{
    m_configuration->fromJson(configuration);
    qDebug() << "Countdown configuration: " << *m_configuration;
}

void Application::startTimer()
{
    calculateTimeRemaining();

    if (!m_timer.isActive()) {
        connect(&m_timer, &QTimer::timeout, this, &Application::calculateTimeRemaining);
        m_timer.start(1000); // Update every second
    }
}

void Application::stopTimer()
{
    if (m_timer.isActive()) {
        disconnect(&m_timer, &QTimer::timeout, this, &Application::calculateTimeRemaining);
        m_timer.stop();
    }
}

void Application::calculateTimeRemaining()
{
    QDateTime now = QDateTime::currentDateTimeUtc();
    QDateTime targetDate = QDateTime::fromSecsSinceEpoch(m_configuration->timestamp());

    qint64 diffSeconds = targetDate.toSecsSinceEpoch() - now.toSecsSinceEpoch();

    if (diffSeconds <= 0) {
        if (!m_finished) {
            setYears(0);
            setDays(0);
            setDaysInWeek(0);
            setWeeks(0);
            setHours(0);
            setMinutes(0);
            setSeconds(0);
            emit countdownFinished();
            m_finished = true;
        }
        emit timeChanged();
        return;
    }

    m_finished = false;

    quint64 years = diffSeconds / (SECONDS_IN_A_DAY * DAYS_IN_YEAR);
    quint64 days = diffSeconds / (SECONDS_IN_A_DAY);
    quint64 daysInWeek = days % DAYS_IN_A_WEEK;
    quint64 weeks = days / DAYS_IN_A_WEEK;
    quint64 hours = (diffSeconds % (SECONDS_IN_A_DAY)) / (SECONDS_IN_HOUR);
    quint64 minutes = (diffSeconds % (SECONDS_IN_HOUR)) / SECONDS_IN_MINUTE;
    quint64 seconds = diffSeconds % SECONDS_IN_MINUTE;

    setYears(years);
    setDays(days);
    setDaysInWeek(daysInWeek);
    setWeeks(weeks);
    setHours(hours);
    setMinutes(minutes);
    setSeconds(seconds);

    emit timeChanged();
}

quint64 Application::years() const
{
    return m_years;
}

void Application::setYears(const quint64 years)
{
    if (m_years != years) {
        m_years = years;
        emit yearsChanged(m_years);
    }
}

quint64 Application::days() const
{
    return m_days;
}

void Application::setDays(const quint64 days)
{
    if (m_days != days) {
        m_days = days;
        emit daysChanged(m_days);
    }
}

quint64 Application::daysInWeek() const
{
    return m_daysInWeek;
}

void Application::setDaysInWeek(const quint64 daysInWeek)
{
    if (m_daysInWeek != daysInWeek) {
        m_daysInWeek = daysInWeek;
        emit daysInWeekChanged(m_daysInWeek);
    }
}

quint64 Application::weeks() const
{
    return m_weeks;
}

void Application::setWeeks(const quint64 weeks)
{
    if (m_weeks != weeks) {
        m_weeks = weeks;
        emit weeksChanged(m_weeks);
    }
}

quint64 Application::hours() const
{
    return m_hours;
}

void Application::setHours(const quint64 hours)
{
    if (m_hours != hours) {
        m_hours = hours;
        emit hoursChanged(m_hours);
    }
}

quint64 Application::minutes() const
{
    return m_minutes;
}

void Application::setMinutes(const quint64 minutes)
{
    if (m_minutes != minutes) {
        m_minutes = minutes;
        emit minutesChanged(m_minutes);
    }
}

quint64 Application::seconds() const
{
    return m_seconds;
}

void Application::setSeconds(const quint64 seconds)
{
    if (m_seconds != seconds) {
        m_seconds = seconds;
        emit secondsChanged(m_seconds);
    }
}
