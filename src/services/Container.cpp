#include "Container.h"
#include "datetime/Service.h"
#include "drivers/Container.h"
using namespace Services;

Container::Container(Drivers::Container& drivers, QObject* parent)
    : QObject(parent),
      m_version(new Version::Service(this)),
      m_remoteApi(new RemoteApi::Service(*drivers.m_network, this)),
      m_notification(new Notification::Service(this)),
      m_media(new Media::Service(*m_remoteApi, this)),
      m_systemMonitor(new SystemMonitor::Service(*m_remoteApi, *drivers.m_temperature, *drivers.m_system, *m_version, *m_notification, this)),
      m_configuration(new Configuration::Service(*m_remoteApi, this)),
      m_dateTime(new DateTime::Service(this)),
      m_qmlInterface(new QmlInterface::Service(this))
{
}

QmlInterface::Service* Container::qmlInterface() const
{
    return m_qmlInterface;
}