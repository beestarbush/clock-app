#ifndef SERVICES_CONFIGURATION_SERVICE_H
#define SERVICES_CONFIGURATION_SERVICE_H

#include "applications/common/Types.h"
#include <QDateTime>
#include <QObject>
#include <QTimer>

class DeviceConfiguration;

namespace Services::RemoteApi
{
class Service;
}

namespace Services::Configuration
{
class Service : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool syncing READ syncing NOTIFY syncingChanged)
    Q_PROPERTY(QDateTime lastSyncTime READ lastSyncTime NOTIFY lastSyncTimeChanged)
    Q_PROPERTY(QString configVersion READ configVersion NOTIFY configVersionChanged)
    Q_PROPERTY(bool startupCheckInProgress READ startupCheckInProgress NOTIFY startupCheckInProgressChanged)

  public:
    explicit Service(Services::RemoteApi::Service& remoteApi, QObject* parent = nullptr);

    // Properties
    bool syncing() const;
    QDateTime lastSyncTime() const;
    QString configVersion() const;
    bool startupCheckInProgress() const;

    DeviceConfiguration* getCurrentConfiguration();

    Q_INVOKABLE void triggerConfigurationChanged();
    void save(const QJsonObject& systemConfiguration, const Common::DynamicApplicationMap& applications);

  signals:
    void syncingChanged();
    void lastSyncTimeChanged();
    void configVersionChanged();
    void configurationChanged();
    void startupCheckInProgressChanged();

  private slots:
    void onSyncTimerTimeout();

  private:
    void loadLocalConfiguration();
    void fetchConfiguration();
    void performStartupCheck();
    void completeStartupCheck();
    void updateCurrentConfig(const DeviceConfiguration& config);

    void setSyncing(bool syncing);
    void setConfigVersion(const QString& version);
    void setStartupCheckInProgress(bool inProgress);

    Services::RemoteApi::Service& m_remoteApi;
    QTimer m_syncTimer;
    QTimer m_startupTimeoutTimer;
    bool m_syncing;
    bool m_startupCheckInProgress;
    QMetaObject::Connection m_startupConnectionWatcher;
    QDateTime m_lastSyncTime;
    QString m_configVersion;
    DeviceConfiguration* m_currentConfig;
};
} // namespace Services::Configuration

#endif // SERVICES_CONFIGURATION_SERVICE_H