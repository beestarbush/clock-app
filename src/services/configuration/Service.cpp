#include "Service.h"
#include "applications/common/Application.h"
#include "applications/common/Configuration.h"
#include "services/remoteapi/DeviceConfiguration.h"
#include "services/remoteapi/Service.h"
#include <QDebug>
#include <QJsonObject>

using namespace Services::Configuration;

#ifdef PLATFORM_IS_TARGET
const QString CONFIGURATION_PATH = QStringLiteral("/usr/share/bee/configuration");
#else
const QString CONFIGURATION_PATH = QStringLiteral("/workdir/build/bee/configuration");
#endif
constexpr int SYNC_INTERVAL_MS = 5 * 60 * 1000; // 5 minutes
constexpr int STARTUP_CHECK_TIMEOUT_MS = 10000; // 10 seconds

Service::Service(Services::RemoteApi::Service& remoteApi, QObject* parent)
    : QObject(parent),
      m_remoteApi(remoteApi),
      m_syncTimer(this),
      m_startupTimeoutTimer(this),
      m_syncing(false),
      m_startupCheckInProgress(false),
      m_currentConfig(nullptr)
{
    connect(&m_syncTimer, &QTimer::timeout, this, &Service::onSyncTimerTimeout);

    m_startupTimeoutTimer.setSingleShot(true);
    m_startupTimeoutTimer.setInterval(STARTUP_CHECK_TIMEOUT_MS);
    connect(&m_startupTimeoutTimer, &QTimer::timeout, this, [this]() {
        if (m_startupCheckInProgress) {
            qWarning() << "Startup check timed out after" << STARTUP_CHECK_TIMEOUT_MS << "ms, using local configuration";
            completeStartupCheck();
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
    qInfo() << "Manual configuration changed triggered!";
    emit configurationChanged();
}

void Service::save(const QJsonObject& systemConfiguration, const Common::DynamicApplicationMap& applications)
{
    DeviceConfiguration newConfig;

    // Preserve existing metadata if available
    if (m_currentConfig && m_currentConfig->isValid()) {
        newConfig.version = m_currentConfig->version;
        newConfig.deviceId = m_currentConfig->deviceId;
        newConfig.activeAppId = m_currentConfig->activeAppId;
    }
    else {
        newConfig.deviceId = m_remoteApi.deviceId();
    }

    newConfig.systemConfiguration = systemConfiguration;

    // Build application configurations from the current in-memory applications
    for (auto it = applications.constBegin(); it != applications.constEnd(); ++it) {
        Common::Application* app = it.value();
        if (!app || !app->configuration()) {
            continue;
        }

        QJsonObject appJson = app->configuration()->toJson();

        // Add metadata
        appJson["id"] = app->id();
        appJson["type"] = Common::typeToString(app->type());
        appJson["name"] = app->displayName();
        appJson["order"] = app->order();
        appJson["watchface"] = Common::watchfaceToString(app->watchface());

        newConfig.addApplication(appJson);
    }

    newConfig.sortApplicationsByOrder();

    updateCurrentConfig(newConfig);
    newConfig.saveToFile(CONFIGURATION_PATH);
    emit configurationChanged();
    qInfo() << "Saving configuration with" << newConfig.applicationCount() << "applications";
}

void Service::onSyncTimerTimeout()
{
    fetchConfiguration();
}

void Service::fetchConfiguration()
{
    if (m_syncing || !m_remoteApi.enabled() || !m_remoteApi.connected()) {
        if (startupCheckInProgress()) {
            completeStartupCheck();
        }
        return;
    }

    setSyncing(true);

    DeviceConfiguration request;
    request.deviceId = m_remoteApi.deviceId();

    m_remoteApi.fetchObject(request,
                            [this](bool success, DeviceConfiguration config, QString error) {
                                setSyncing(false);

                                if (!success) {
                                    qWarning() << "Failed to fetch configuration:" << error;
                                    if (startupCheckInProgress()) {
                                        completeStartupCheck();
                                    }
                                    return;
                                }

                                if (config.version != m_configVersion) {
                                    qInfo() << "Configuration version changed from"
                                            << m_configVersion << "to" << config.version;

                                    updateCurrentConfig(config);
                                    config.saveToFile(CONFIGURATION_PATH);
                                    setConfigVersion(config.version);
                                    if (!startupCheckInProgress()) {
                                        emit configurationChanged();
                                    }
                                }

                                m_lastSyncTime = QDateTime::currentDateTime();
                                emit lastSyncTimeChanged();

                                if (startupCheckInProgress()) {
                                    completeStartupCheck();
                                }
                            });
}

void Service::performStartupCheck()
{
    if (startupCheckInProgress()) {
        return;
    }

    setStartupCheckInProgress(true);

    // Always load local configuration as baseline
    loadLocalConfiguration();

    if (!m_remoteApi.enabled()) {
        qInfo() << "Remote API disabled, using local configuration";
        completeStartupCheck();
        return;
    }

    m_startupTimeoutTimer.start();

    if (m_remoteApi.connected()) {
        fetchConfiguration();
        return;
    }

    qInfo() << "Waiting for remote API connection (max" << STARTUP_CHECK_TIMEOUT_MS << "ms)...";
    m_startupConnectionWatcher = connect(&m_remoteApi,
                                         &Services::RemoteApi::Service::connectedChanged,
                                         this,
                                         [this]() {
                                             if (m_remoteApi.connected() && m_startupCheckInProgress) {
                                                 disconnect(m_startupConnectionWatcher);
                                                 fetchConfiguration();
                                             }
                                         });
}

void Service::completeStartupCheck()
{
    m_startupTimeoutTimer.stop();
    disconnect(m_startupConnectionWatcher);
    setStartupCheckInProgress(false);

    // Notify that configuration is available (even if unchanged, apps need to load)
    emit configurationChanged();

    m_syncTimer.start(SYNC_INTERVAL_MS);
    qInfo() << "Startup check complete. Periodic sync every" << (SYNC_INTERVAL_MS / 1000) << "seconds";
}

void Service::loadLocalConfiguration()
{
    DeviceConfiguration config = DeviceConfiguration::loadFromFile(CONFIGURATION_PATH);

    if (config.isValid() && config.hasApplications()) {
        qInfo() << "Loaded local configuration, version:" << config.version;
        updateCurrentConfig(config);
        setConfigVersion(config.version);
    }
    else {
        qInfo() << "No valid local configuration found";
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
