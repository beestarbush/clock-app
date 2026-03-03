#include "Application.h"
#include <QDateTime>
using namespace Applications::TimeElapsed;

Application::Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media, QObject* parent)
    : Common::TimerApplication(id, type, displayName, order, watchface, media, parent)
{
    startTimer();
}

void Application::calculateTime()
{
    QDateTime now = QDateTime::currentDateTimeUtc();
    QDateTime referenceDate = QDateTime::fromSecsSinceEpoch(m_configuration->timestamp());

    // Calculate the difference in seconds; clamp to zero if reference is in the future
    qint64 diffSigned = now.toSecsSinceEpoch() - referenceDate.toSecsSinceEpoch();
    quint64 diff = diffSigned > 0 ? static_cast<quint64>(diffSigned) : 0;

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

