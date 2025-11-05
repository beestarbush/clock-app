#include "Application.h"
#include "services/media/Service.h"
#include <QDebug>
using namespace Applications::Clock;

Application::Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media, QObject* parent)
    : Common::Application(id, type, displayName, order, watchface, parent),
      m_configuration(new Configuration(id, parent)),
      m_media(media)
{
    // Refresh background when media sync completes
    connect(&m_media, &Services::Media::Service::syncCompleted, this, [this]() {
        emit m_configuration->backgroundChanged();
    });
}

Configuration* Application::configuration() const
{
    return m_configuration;
}

void Application::applyConfiguration(const Configuration& configuration)
{
    *m_configuration = configuration;
    qDebug() << "Clock configuration: " << *m_configuration;
}

void Application::applyConfiguration(const QJsonObject& configuration)
{
    m_configuration->fromJson(configuration);
    qDebug() << "Clock configuration: " << *m_configuration;
}