#include "Application.h"
#include "services/datetime/Service.h"
#include "services/media/Service.h"
#include <QDateTime>
#include <QDebug>
#include <QTimeZone>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(CurrentDateApplication, "CurrentDateApplication")

using namespace Applications::CurrentDate;

Application::Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media, Services::DateTime::Service& dateTime, QObject* parent)
    : Common::Application(id, type, displayName, order, watchface, parent),
      m_configuration(new Common::Configuration(id, parent)),
      m_media(media),
      m_dateTime(dateTime),
      m_updateTimer(new QTimer(this))
{
    // Refresh background when media sync completes
    connect(&m_media, &Services::Media::Service::syncCompleted, this, [this]() {
        emit m_configuration->backgroundChanged();
    });

    // Update the date every 60 seconds
    m_updateTimer->setInterval(60000);
    connect(m_updateTimer, &QTimer::timeout, this, &Application::notifyDateChanged);
    m_updateTimer->start();
}

Common::Configuration* Application::configuration() const
{
    return m_configuration;
}

void Application::applyConfiguration(const QJsonObject& configuration)
{
    m_configuration->fromJson(configuration);
    qCDebug(CurrentDateApplication) << "Configuration: " << *m_configuration;
}

QString Application::currentDate() const
{
    QDateTime now = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone("Europe/Amsterdam"));
    return now.toString("dddd\nd MMMM yyyy");
}

int Application::year() const
{
    QDateTime now = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone("Europe/Amsterdam"));
    return now.date().year();
}

int Application::month() const
{
    QDateTime now = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone("Europe/Amsterdam"));
    return now.date().month();
}

int Application::day() const
{
    QDateTime now = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone("Europe/Amsterdam"));
    return now.date().day();
}

int Application::dayOfWeek() const
{
    QDateTime now = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone("Europe/Amsterdam"));
    return now.date().dayOfWeek();
}

void Application::notifyDateChanged()
{
    emit currentDateChanged();
    emit yearChanged();
    emit monthChanged();
    emit dayChanged();
    emit dayOfWeekChanged();
}
