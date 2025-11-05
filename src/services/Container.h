#ifndef SERVICES_CONTAINER_H
#define SERVICES_CONTAINER_H

#include <QObject>

#include "configuration/Service.h"
#include "datetime/Service.h"
#include "media/Service.h"
#include "notification/Service.h"
#include "qmlinterface/Service.h"
#include "remoteapi/Service.h"
#include "systemmonitor/Service.h"
#include "version/Service.h"

namespace Drivers
{
class Container;
}

namespace Applications
{
class Container;
}

namespace Services
{
class Container : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Services::Media::Service* media MEMBER m_media CONSTANT)
    Q_PROPERTY(Services::DateTime::Service* dateTime MEMBER m_dateTime CONSTANT)
    Q_PROPERTY(Services::Notification::Service* notification MEMBER m_notification CONSTANT)
    Q_PROPERTY(Services::QmlInterface::Service* qmlInterface MEMBER m_qmlInterface CONSTANT)
    Q_PROPERTY(Services::Version::Service* version MEMBER m_version CONSTANT)
    Q_PROPERTY(Services::RemoteApi::Service* remoteApi MEMBER m_remoteApi CONSTANT)
    Q_PROPERTY(Services::SystemMonitor::Service* systemMonitor MEMBER m_systemMonitor CONSTANT)
    Q_PROPERTY(Services::Configuration::Service* configuration MEMBER m_configuration CONSTANT)
  public:
    friend class ::Applications::Container;

    Container(Drivers::Container& drivers, QObject* parent = nullptr);

    QmlInterface::Service* qmlInterface() const;

  private:
    Version::Service* m_version;
    RemoteApi::Service* m_remoteApi;
    Notification::Service* m_notification;
    Media::Service* m_media;
    SystemMonitor::Service* m_systemMonitor;
    Configuration::Service* m_configuration;
    DateTime::Service* m_dateTime;
    QmlInterface::Service* m_qmlInterface;
};
} // namespace Services

#endif // SERVICES_CONTAINER_H