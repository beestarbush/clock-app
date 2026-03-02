#include "Service.h"
#include "drivers/system/Driver.h"
#include "git_version.h"
#include "services/notification/Service.h"
#include "services/websocket/Service.h"
#include "services/websocket/Types.h"
#include "services/version/Service.h"

#include <QDebug>
#include <QJsonObject>
using namespace Services::SystemMonitor;

constexpr int MONITOR_INTERVAL = 10 * 1000;    // 10 seconds
constexpr int REPORT_INTERVAL = 5 * 60 * 1000; // 5 minutes

Service::Service(Services::WebSocket::Service& webSocket,
                 Drivers::System::Driver& system,
                 Version::Service& version,
                 Notification::Service& notificationManager,
                 QObject* parent)
    : QObject(parent),
      m_webSocket(webSocket),
      m_system(system),
      m_version(version),
      m_notificationManager(notificationManager),
      m_temperature(0),
      m_isReporting(false)
{
    // Subscribe to temperature data from backend
    m_webSocket.subscribe(Services::WebSocket::Topic::Temperature);
    connect(&m_webSocket, &Services::WebSocket::Service::publishReceived,
        this, [this](const Services::WebSocket::Topic& topic, const QJsonObject& data) {
            if (topic == Services::WebSocket::Topic::Temperature) {
                onTemperatureReceived(data);
            }
        });

    // Configure monitor timer (temperature checks etc.)
    m_monitorTimer.setSingleShot(false);
    m_monitorTimer.setInterval(MONITOR_INTERVAL);
    connect(&m_monitorTimer, &QTimer::timeout, this, &Service::monitor);
    monitor();

    // Configure report timer (publish status to backend)
    m_reportTimer.setSingleShot(false);
    m_reportTimer.setInterval(REPORT_INTERVAL);
    connect(&m_reportTimer, &QTimer::timeout, this, &Service::report);

    // Start reporting once WebSocket is connected
    connect(&m_webSocket, &Services::WebSocket::Service::connectedChanged, this, [this]() {
        if (m_webSocket.connected()) {
            report();
            m_reportTimer.start();
        } else {
            m_reportTimer.stop();
        }
    });

    if (m_webSocket.connected()) {
        report();
        m_reportTimer.start();
    }
}

void Service::onTemperatureReceived(const QJsonObject& data)
{
    m_temperature = data["temperature"].toDouble(0);
}

void Service::monitor()
{
    if (m_temperature > 85000) { // 85.0 °C
        m_notificationManager.showWarning(
            QStringLiteral("High CPU temperature"),
            QStringLiteral("The CPU temperature is too high. Please ensure proper cooling."));
    }

    m_monitorTimer.start();
}

void Service::report()
{
    QJsonObject status;
    status["version"] = m_version.tag();
    status["uptime"] = static_cast<double>(m_system.uptimeSeconds());
    m_webSocket.publish(Services::WebSocket::Topic::ApplicationStatus, status);
    qDebug() << "Published status update to backend";
}
