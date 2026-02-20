#ifndef SERVICES_CONFIGURATION_SERVICE_H
#define SERVICES_CONFIGURATION_SERVICE_H

#include "applications/common/Types.h"
#include <QDateTime>
#include <QObject>
#include <QTimer>

#include "DeviceConfiguration.h"

namespace Services::WebSocket
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
    explicit Service(Services::WebSocket::Service& webSocket, QObject* parent = nullptr);

    DeviceConfiguration* getCurrentConfiguration();

    // Trigger signals
    void triggerConfigurationChanged();

    // Properties
    bool syncing() const;
    QDateTime lastSyncTime() const;
    QString configVersion() const;
    bool startupCheckInProgress() const;

  signals:
    void configurationChanged();
    void syncingChanged();
    void lastSyncTimeChanged();
    void configVersionChanged();
    void startupCheckInProgressChanged();

  private:
    void performStartupCheck();
    void completeStartupCheck();
    void loadLocalConfiguration();
    void updateCurrentConfig(const DeviceConfiguration& config);

    void setSyncing(bool syncing);
    void setConfigVersion(const QString& version);
    void setStartupCheckInProgress(bool inProgress);
    
    // Slot for incoming config data
    void onConfigurationReceived(const QJsonObject& configJson);

    Services::WebSocket::Service& m_webSocket;
    QTimer m_startupTimeoutTimer;
    QMetaObject::Connection m_startupConnectionWatcher;

    bool m_syncing;
    bool m_startupCheckInProgress;
    QString m_configVersion;
    QDateTime m_lastSyncTime;
    DeviceConfiguration* m_currentConfig;
};
} // namespace Services::Configuration

#endif // SERVICES_CONFIGURATION_SERVICE_H
