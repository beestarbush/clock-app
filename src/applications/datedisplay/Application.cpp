#include "Application.h"
#include "services/datetime/Service.h"
#include "services/media/Service.h"
#include <QDateTime>
#include <QDebug>
#include <QTimeZone>
using namespace Applications::DateDisplay;

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
    connect(m_updateTimer, &QTimer::timeout, this, &Application::currentDateChanged);
    m_updateTimer->start();
}

Common::Configuration* Application::configuration() const
{
    return m_configuration;
}

void Application::applyConfiguration(const QJsonObject& configuration)
{
    m_configuration->fromJson(configuration);
    qDebug() << "DateDisplay configuration: " << *m_configuration;
}

QString Application::currentDate() const
{
    QDateTime now = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone("Europe/Amsterdam"));
    return now.toString("dddd\nd MMMM yyyy");
}
