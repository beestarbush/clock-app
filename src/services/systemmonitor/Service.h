#ifndef SERVICES_SYSTEMMONITOR_SERVICE_H
#define SERVICES_SYSTEMMONITOR_SERVICE_H

#include "websocket/client/Service.h"
#include <QObject>
#include <QTimer>
namespace Services::Version
{
class Service;
}
namespace Services::Notification
{
class Service;
}

namespace Services::SystemMonitor
{
/**
 * Service
 *
 * Periodically monitors system status (processor temperature, uptime, etc.) and
 * reports it to the remote API server. Subscribes to the backend for
 * processor temperature data.
 */
class Service : public QObject
{
    Q_OBJECT

  public:
    explicit Service(Common::Communication::WebSocket::Client::Service& webSocket,
                     Services::Version::Service& version,
                     Services::Notification::Service& notificationManager,
                     QObject* parent = nullptr);

  private:
    void monitor();
    void report();
    void onProcessorTemperatureReceived(const QJsonObject& data);

    Common::Communication::WebSocket::Client::Service& m_webSocket;
    Services::Version::Service& m_version;
    Services::Notification::Service& m_notificationManager;

    double m_processorTemperature;
    QTimer m_monitorTimer;
    QTimer m_reportTimer;
    bool m_isReporting;
};
} // namespace Services::SystemMonitor

#endif // SERVICES_SYSTEMMONITOR_SERVICE_H
