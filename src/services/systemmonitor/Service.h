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
 * reports it to the remote API server. Subscribes to the backend for
 * temperature data.
 */
class Service : public QObject
{
    Q_OBJECT

  public:
    explicit Service(Services::WebSocket::Service& webSocket,
                     Drivers::System::Driver& system,
                     Services::Version::Service& version,
                     Services::Notification::Service& notificationManager,
                     QObject* parent = nullptr);

  private:
    void monitor();
    void report();
    void onTemperatureReceived(const QJsonObject& data);

    Services::WebSocket::Service& m_webSocket;
    Drivers::System::Driver& m_system;
    Services::Version::Service& m_version;
    Services::Notification::Service& m_notificationManager;

    double m_temperature;
    QTimer m_monitorTimer;
    QTimer m_reportTimer;
    bool m_isReporting;
};
} // namespace Services::SystemMonitor

#endif // SERVICES_SYSTEMMONITOR_SERVICE_H
