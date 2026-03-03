#ifndef COMMON_TIMER_APPLICATION_H
#define COMMON_TIMER_APPLICATION_H

#include "Application.h"
#include "TimerConfiguration.h"
#include <QTimer>

namespace Services::Media
{
class Service;
}

namespace Common
{
/**
 * Base class for timer-based applications (Countdown, TimeElapsed).
 * Manages the shared timer logic, time component properties, and configuration.
 * Subclasses implement calculateTime() to compute the specific time values.
 */
class TimerApplication : public Application
{
    Q_OBJECT
    Q_PROPERTY(Common::TimerConfiguration* configuration READ configuration CONSTANT)
    Q_PROPERTY(quint64 years READ years NOTIFY timeChanged)
    Q_PROPERTY(quint64 days READ days NOTIFY timeChanged)
    Q_PROPERTY(quint64 daysInWeek READ daysInWeek NOTIFY timeChanged)
    Q_PROPERTY(quint64 weeks READ weeks NOTIFY timeChanged)
    Q_PROPERTY(quint64 hours READ hours NOTIFY timeChanged)
    Q_PROPERTY(quint64 minutes READ minutes NOTIFY timeChanged)
    Q_PROPERTY(quint64 seconds READ seconds NOTIFY timeChanged)

  public:
    TimerApplication(const QString& id,
                     Type type,
                     const QString& displayName,
                     int order,
                     Watchface watchface,
                     Services::Media::Service& media,
                     QObject* parent = nullptr);

    Common::TimerConfiguration* configuration() const override;

    void applyConfiguration(const Common::TimerConfiguration& configuration);
    void applyConfiguration(const QJsonObject& configuration) override;

    quint64 years() const;
    quint64 days() const;
    quint64 daysInWeek() const;
    quint64 weeks() const;
    quint64 hours() const;
    quint64 minutes() const;
    quint64 seconds() const;

    void startTimer();
    void stopTimer();

  signals:
    void timeChanged();
    void yearsChanged(quint64 years);
    void daysChanged(quint64 days);
    void daysInWeekChanged(quint64 daysInWeek);
    void weeksChanged(quint64 weeks);
    void hoursChanged(quint64 hours);
    void minutesChanged(quint64 minutes);
    void secondsChanged(quint64 seconds);

  protected:
    virtual void calculateTime() = 0;

    void setYears(quint64 years);
    void setDays(quint64 days);
    void setDaysInWeek(quint64 daysInWeek);
    void setWeeks(quint64 weeks);
    void setHours(quint64 hours);
    void setMinutes(quint64 minutes);
    void setSeconds(quint64 seconds);

    // Shared time constants available to subclasses
    static constexpr quint64 SECONDS_IN_MINUTE = 60;
    static constexpr quint64 SECONDS_IN_HOUR = SECONDS_IN_MINUTE * 60;
    static constexpr quint64 SECONDS_IN_A_DAY = SECONDS_IN_HOUR * 24;
    static constexpr quint64 DAYS_IN_A_WEEK = 7;
    static constexpr quint64 DAYS_IN_YEAR = 365;

    Common::TimerConfiguration* m_configuration;
    Services::Media::Service& m_media;
    QTimer m_timer;

  private:
    quint64 m_years = 0;
    quint64 m_days = 0;
    quint64 m_daysInWeek = 0;
    quint64 m_weeks = 0;
    quint64 m_hours = 0;
    quint64 m_minutes = 0;
    quint64 m_seconds = 0;
};
} // namespace Common

#endif // COMMON_TIMER_APPLICATION_H
