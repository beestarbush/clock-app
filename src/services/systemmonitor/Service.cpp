#include "Service.h"
#include "git_version.h"
#include "services/notification/Service.h"
#include "services/version/Service.h"
#include "services/websocket/Service.h"
#include "services/websocket/Types.h"

#include <QDebug>
#include <QJsonObject>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(SystemMonitorService, "SystemMonitorService")

using namespace Services::SystemMonitor;

constexpr int MONITOR_INTERVAL = 10 * 1000;    // 10 seconds
constexpr int REPORT_INTERVAL = 5 * 60 * 1000; // 5 minutes

Service::Service(Services::WebSocket::Service& webSocket,
                 Version::Service& version,
                 Notification::Service& notificationManager,
                 QObject* parent)
    : QObject(parent),
      m_webSocket(webSocket),
      m_version(version),
      m_notificationManager(notificationManager),
      m_processorTemperature(0),
      m_isReporting(false)
{
    // Subscribe to processor temperature data from backend
    m_webSocket.subscribe(Services::WebSocket::Topic::ProcessorTemperature);
    connect(&m_webSocket, &Services::WebSocket::Service::publishReceived, this, [this](const Services::WebSocket::Topic& topic, const QJsonObject& data) {
        if (topic == Services::WebSocket::Topic::ProcessorTemperature) {
            onProcessorTemperatureReceived(data);
        }
    });

    // Configure monitor timer (processor temperature checks etc.)
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
            // Fetch initial processor temperature
            m_webSocket.request(Services::WebSocket::Method::GetProcessorTemperature, {}, [this](bool success, const QJsonObject& response, const QString&) {
                if (success && response.contains("processor_temperature")) {
                    onProcessorTemperatureReceived(response);
                }
            });
            report();
            m_reportTimer.start();
        }
        else {
            m_reportTimer.stop();
        }
    });

    if (m_webSocket.connected()) {
        report();
        m_reportTimer.start();
    }
}

void Service::onProcessorTemperatureReceived(const QJsonObject& data)
{
    m_processorTemperature = data["processor_temperature"].toDouble(0);
}

void Service::monitor()
{
    if (m_processorTemperature > 85000) { // 85.0 °C
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
    m_webSocket.publish(Services::WebSocket::Topic::ApplicationStatus, status);
    qCDebug(SystemMonitorService) << "Published status update to backend";
}
