#ifndef SERVICES_SYSTEMMONITOR_SERVICE_H
#define SERVICES_SYSTEMMONITOR_SERVICE_H

#include <QObject>
#include <QTimer>

namespace Services::WebSocket
{
class Service;
}
namespace Services::Version
{
class Service;
}
namespace Services::Notification
{
class Service;
}
namespace Drivers::Temperature
{
class Driver;
}
namespace Drivers::System
{
class Driver;
}

namespace Services::SystemMonitor
{
/**
 * Service
 *
 * Periodically monitors system status (temperature, uptime, etc.) and
 * reports it to the remote API server.
 */
class Service : public QObject
{
    Q_OBJECT

  public:
    explicit Service(Services::WebSocket::Service& webSocket,
                     Drivers::Temperature::Driver& temperature,
                     Drivers::System::Driver& system,
                     Services::Version::Service& version,
                     Services::Notification::Service& notificationManager,
                     QObject* parent = nullptr);

  private:
    void monitor();
    void report();

    Services::WebSocket::Service& m_webSocket;
    Drivers::Temperature::Driver& m_temperature;
    Drivers::System::Driver& m_system;
    Services::Version::Service& m_version;
    Services::Notification::Service& m_notificationManager;

    QTimer m_monitorTimer;
    QTimer m_reportTimer;
    bool m_isReporting;
};
} // namespace Services::SystemMonitor

#endif // SERVICES_SYSTEMMONITOR_SERVICE_H
