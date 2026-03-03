#include "Application.h"
#include <QDateTime>
using namespace Applications::Countdown;

Application::Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media, QObject* parent)
    : Common::TimerApplication(id, type, displayName, order, watchface, media, parent)
{
    startTimer();
}

void Application::calculateTime()
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

    quint64 diff = static_cast<quint64>(diffSeconds);
    quint64 days = diff / SECONDS_IN_A_DAY;

    setYears(diff / (SECONDS_IN_A_DAY * DAYS_IN_YEAR));
    setDays(days);
    setDaysInWeek(days % DAYS_IN_A_WEEK);
    setWeeks(days / DAYS_IN_A_WEEK);
    setHours((diff % SECONDS_IN_A_DAY) / SECONDS_IN_HOUR);
    setMinutes((diff % SECONDS_IN_HOUR) / SECONDS_IN_MINUTE);
    setSeconds(diff % SECONDS_IN_MINUTE);

    emit timeChanged();
}

