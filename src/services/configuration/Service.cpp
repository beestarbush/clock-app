#include "Service.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(ConfigurationService, "ConfigurationService")

using namespace Services::Configuration;

constexpr int STARTUP_CHECK_TIMEOUT_MS = 10000; // 10 seconds

Service::Service(Common::Communication::WebSocket::Client::Service& webSocket, QObject* parent)
    : QObject(parent),
      m_webSocket(webSocket),
      m_startupTimeoutTimer(this),
      m_syncing(false),
      m_startupCheckInProgress(false),
      m_currentConfig(nullptr)
{
    m_webSocket.subscribe(Common::Communication::WebSocket::Topic::Configuration,
                          QJsonObject(),
                          this,
                          [this](const QJsonObject& data) {
                              onSystemConfigReceived(data);
                          });

    connect(&m_webSocket, &Common::Communication::WebSocket::Client::Service::connectedChanged, this, [this]() {
        if (!m_webSocket.connected()) {
            return;
        }

        // Subscriptions are tracked by the websocket client and automatically re-subscribed on reconnect.
    });

    performStartupCheck();
}

DeviceConfiguration* Service::getCurrentConfiguration()
{
    return m_currentConfig;
}

void Service::triggerConfigurationChanged()
{
    qCInfo(ConfigurationService) << "Manual configuration changed triggered!";
    emit configurationChanged();
}

void Service::onSystemConfigReceived(const QJsonObject& payload)
{
    const QJsonObject systemConfig = payload.value("system-configuration").toObject();
    const QString deviceId = payload.value("device_id").toString();

    if (systemConfig.isEmpty()) {
        qCWarning(ConfigurationService) << "Received invalid system configuration payload:" << QString::fromUtf8(QJsonDocument(payload).toJson(QJsonDocument::Compact));
        return;
    }

    setSyncing(true);

    if (!m_currentConfig) {
        updateCurrentConfig(DeviceConfiguration());
    }

    m_currentConfig->systemConfiguration = systemConfig;
    m_currentConfig->deviceId = deviceId;

    emit systemConfigChanged(deviceId, systemConfig);

    m_lastSyncTime = QDateTime::currentDateTime();
    emit lastSyncTimeChanged();

    if (startupCheckInProgress()) {
        completeStartupCheck();
    }

    emit configurationChanged();
    setSyncing(false);
}

void Service::performStartupCheck()
{
    if (startupCheckInProgress())
        return;

    setSyncing(true);
    setStartupCheckInProgress(true);

    m_startupTimeoutTimer.setSingleShot(true);
    m_startupTimeoutTimer.setInterval(STARTUP_CHECK_TIMEOUT_MS);
    connect(&m_startupTimeoutTimer, &QTimer::timeout, this, [this]() {
        if (m_startupCheckInProgress) {
            qCWarning(ConfigurationService) << "Startup check timed out after" << STARTUP_CHECK_TIMEOUT_MS << "ms";
            completeStartupCheck();
        }
    });
    m_startupTimeoutTimer.start();

    if (m_webSocket.connected()) {
        return;
    }

    qCInfo(ConfigurationService) << "Waiting for WebSocket connection (max" << STARTUP_CHECK_TIMEOUT_MS << "ms)...";

    m_startupConnectionWatcher = connect(&m_webSocket,
                                         &Common::Communication::WebSocket::Client::Service::connectedChanged,
                                         this,
                                         [this]() {
                                             if (m_webSocket.connected() && startupCheckInProgress()) {
                                                 disconnect(m_startupConnectionWatcher);
                                             }
                                         });
}

void Service::completeStartupCheck()
{
    m_startupTimeoutTimer.stop();
    disconnect(m_startupConnectionWatcher);
    setStartupCheckInProgress(false);
    setSyncing(false);

    emit configurationChanged();
    qCInfo(ConfigurationService) << "Startup check complete. Waiting for push updates.";
}

void Service::updateCurrentConfig(const DeviceConfiguration& config)
{
    if (m_currentConfig) {
        delete m_currentConfig;
    }
    m_currentConfig = new DeviceConfiguration(config);
}

bool Service::syncing() const
{
    return m_syncing;
}

QDateTime Service::lastSyncTime() const
{
    return m_lastSyncTime;
}

QString Service::configVersion() const
{
    return m_configVersion;
}

bool Service::startupCheckInProgress() const
{
    return m_startupCheckInProgress;
}

void Service::setSyncing(bool syncing)
{
    if (m_syncing != syncing) {
        m_syncing = syncing;
        emit syncingChanged();
    }
}

void Service::setConfigVersion(const QString& version)
{
    if (m_configVersion != version) {
        m_configVersion = version;
        emit configVersionChanged();
    }
}

void Service::setStartupCheckInProgress(bool inProgress)
{
    if (m_startupCheckInProgress != inProgress) {
        m_startupCheckInProgress = inProgress;
        emit startupCheckInProgressChanged();
    }
}
