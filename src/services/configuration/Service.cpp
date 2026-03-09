#include "Service.h"
#include "applications/common/Application.h"
#include "applications/common/Configuration.h"
#include "services/websocket/Service.h"
#include <QDebug>
#include <QJsonObject>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(ConfigurationService, "ConfigurationService")

using namespace Services::Configuration;

#ifdef PLATFORM_IS_TARGET
const QString CONFIGURATION_PATH = QStringLiteral(".");
#else
const QString CONFIGURATION_PATH = QStringLiteral("/workdir/build/bee/configuration");
#endif

constexpr int STARTUP_CHECK_TIMEOUT_MS = 10000; // 10 seconds

Service::Service(Services::WebSocket::Service& webSocket, QObject* parent)
    : QObject(parent),
      m_webSocket(webSocket),
      m_startupTimeoutTimer(this),
      m_syncing(false),
      m_startupCheckInProgress(false),
      m_currentConfig(nullptr)
{
    // Subscribe to config change notifications
    m_webSocket.subscribe(Services::WebSocket::Topic::Configuration);
    connect(&m_webSocket, &Services::WebSocket::Service::publishReceived, this, [this](const Services::WebSocket::Topic& topic, const QJsonObject& data) {
        if (topic == Services::WebSocket::Topic::Configuration) {
            onConfigurationReceived(data);
        }
    });
    connect(&m_webSocket, &Services::WebSocket::Service::connectedChanged, this, [this]() {
        if (m_webSocket.connected()) {
            m_webSocket.request(Services::WebSocket::Method::GetConfig, QJsonObject(), [this](bool success, const QJsonObject& result, const QString& error) {
                if (success) {
                    onConfigurationReceived(result);
                } else {
                    qCWarning(ConfigurationService) << "Failed to get config:" << error;
                }
            });
        }
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

void Service::onConfigurationReceived(const QJsonObject& configJson)
{
    qCInfo(ConfigurationService) << "Received configuration JSON:" << QString::fromUtf8(QJsonDocument(configJson).toJson(QJsonDocument::Compact));
    if (configJson.isEmpty()) {
        qCWarning(ConfigurationService) << "Received empty configuration JSON";
        return;
    }

    // Validate if it looks like a config
    if (!configJson.contains("system-configuration")){
        qCWarning(ConfigurationService) << "Received invalid configuration JSON";
        return;
    }
    setSyncing(true); // Indicate we are processing a new config

    DeviceConfiguration config = DeviceConfiguration::fromJson(configJson);
    updateCurrentConfig(config);
    //config.saveToFile(CONFIGURATION_PATH); // Cache locally
    setConfigVersion(config.version);
    if (!startupCheckInProgress()) {
        emit configurationChanged();
    }

    m_lastSyncTime = QDateTime::currentDateTime();
    emit lastSyncTimeChanged();

    if (startupCheckInProgress()) {
         completeStartupCheck();
    }
    setSyncing(false);
}

void Service::performStartupCheck()
{
    if (startupCheckInProgress()) return;

    setSyncing(true); // Indicate we are trying to sync
    setStartupCheckInProgress(true);

    m_startupTimeoutTimer.setSingleShot(true);
    m_startupTimeoutTimer.setInterval(STARTUP_CHECK_TIMEOUT_MS);
    connect(&m_startupTimeoutTimer, &QTimer::timeout, this, [this]() {
        if (m_startupCheckInProgress) {
            qCWarning(ConfigurationService) << "Startup check timed out after" << STARTUP_CHECK_TIMEOUT_MS << "ms, using local configuration";
            completeStartupCheck();
        }
    });
    m_startupTimeoutTimer.start();

    // If already connected, request config immediately
    if (m_webSocket.connected()) {
        m_webSocket.request(Services::WebSocket::Method::GetConfig, QJsonObject(), [this](bool success, const QJsonObject& result, const QString& error) {
            if (!success) {
                qCWarning(ConfigurationService) << "Failed to get config:" << error;
                return;
            }

            onConfigurationReceived(result);
        });
        return;
    }

    qCInfo(ConfigurationService) << "Waiting for WebSocket connection (max" << STARTUP_CHECK_TIMEOUT_MS << "ms)...";
    
    m_startupConnectionWatcher = connect(&m_webSocket,
                                         &Services::WebSocket::Service::connectedChanged,
                                         this,
                                         [this]() {
                                             if (m_webSocket.connected() && startupCheckInProgress()) {
                                                 disconnect(m_startupConnectionWatcher);
                                                 m_webSocket.request(Services::WebSocket::Method::GetConfig, QJsonObject(), [this](bool success, const QJsonObject& result, const QString& error) {
                                                     if (success) {
                                                         onConfigurationReceived(result);
                                                     } else {
                                                        qCWarning(ConfigurationService) << "Failed to get config:" << error;
                                                     }
                                                 });
                                             }
                                         });
}

void Service::completeStartupCheck()
{
    m_startupTimeoutTimer.stop();
    disconnect(m_startupConnectionWatcher);
    setStartupCheckInProgress(false);
    setSyncing(false);

    if (!m_currentConfig || !m_currentConfig->isValid()) {
        //loadLocalConfiguration();
        qCInfo(ConfigurationService) << "No valid configuration loaded during startup check";
    }

    emit configurationChanged();
    qCInfo(ConfigurationService) << "Startup check complete. Waiting for push updates.";
}

void Service::loadLocalConfiguration()
{
    DeviceConfiguration config = DeviceConfiguration::loadFromFile(CONFIGURATION_PATH);

    if (config.isValid() && config.hasApplications()) {
        qCInfo(ConfigurationService) << "Loaded local configuration, version:" << config.version;
        updateCurrentConfig(config);
        setConfigVersion(config.version);
    }
    else {
        qCInfo(ConfigurationService) << "No valid local configuration found";
    }
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
