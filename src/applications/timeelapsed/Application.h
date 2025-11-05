#ifndef APPS_TIME_ELAPSED_APPLICATION_H
#define APPS_TIME_ELAPSED_APPLICATION_H

#include "applications/common/Application.h"
#include "applications/common/TimerConfiguration.h"
#include <QObject>
#include <QTimer>

namespace Services::Media
{
class Service;
}

namespace Applications::TimeElapsed
{
class Application : public Common::Application
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
    Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media, QObject* parent = nullptr);

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

  private:
    void calculateTimeElapsed();
    void setYears(const quint64 years);
    void setDays(const quint64 days);
    void setDaysInWeek(const quint64 daysInWeek);
    void setWeeks(const quint64 weeks);
    void setHours(const quint64 hours);
    void setMinutes(const quint64 minutes);
    void setSeconds(const quint64 seconds);

    Common::TimerConfiguration* m_configuration;
    Services::Media::Service& m_media;

    // Properties for indicating the elapsed time
    quint64 m_years;
    quint64 m_days;
    quint64 m_daysInWeek;
    quint64 m_weeks;
    quint64 m_hours;
    quint64 m_minutes;
    quint64 m_seconds;

    QTimer m_timer;
};
} // namespace Applications::TimeElapsed

#endif // APPS_TIME_ELAPSED_APPLICATION_H
