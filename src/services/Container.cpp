#include "Container.h"
#include "datetime/Service.h"
#include "drivers/Container.h"
using namespace Services;

Container::Container(Drivers::Container& drivers, QObject* parent)
    : QObject(parent),
      m_version(new Version::Service(this)),
      m_rest(new Rest::Service(*drivers.m_network, this)),
      m_websocket(new WebSocket::Service(*drivers.m_network, this)),
      m_notification(new Notification::Service(this)),
      m_media(new Media::Service(*m_websocket, *m_rest, this)),
      m_systemMonitor(new SystemMonitor::Service(*m_websocket, *drivers.m_temperature, *drivers.m_system, *m_version, *m_notification, this)),
      m_configuration(new Configuration::Service(*m_websocket, this)),
      m_dateTime(new DateTime::Service(this)),
      m_qmlInterface(new QmlInterface::Service(this))
{
}

QmlInterface::Service* Container::qmlInterface() const
{
    return m_qmlInterface;
}
