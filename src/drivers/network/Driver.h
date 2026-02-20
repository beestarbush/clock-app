#ifndef DRIVERS_NETWORK_DRIVER_H
#define DRIVERS_NETWORK_DRIVER_H

#include <QObject>
#include <QTimer>

namespace Drivers::Network
{
class Driver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString externalInterfaceName READ externalInterfaceName NOTIFY externalInterfaceNameChanged)
    Q_PROPERTY(QString loopbackInterfaceName READ loopbackInterfaceName NOTIFY loopbackInterfaceNameChanged)
    Q_PROPERTY(bool externalInterfaceConnected READ externalInterfaceConnected NOTIFY externalInterfaceConnectedChanged)
    Q_PROPERTY(bool loopbackInterfaceConnected READ loopbackInterfaceConnected NOTIFY loopbackInterfaceConnectedChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(QString ipAddress READ ipAddress NOTIFY ipAddressChanged)
    Q_PROPERTY(QString subnetMask READ subnetMask NOTIFY subnetMaskChanged)

  public:
    Driver(QObject* parent = nullptr);

    QString externalInterfaceName() const;
    QString loopbackInterfaceName() const;
    bool externalInterfaceConnected() const;
    bool loopbackInterfaceConnected() const;
    bool running() const;
    QString ipAddress() const;
    QString subnetMask() const;

  signals:
    void externalInterfaceNameChanged();
    void loopbackInterfaceNameChanged();
    void externalInterfaceConnectedChanged();
    void loopbackInterfaceConnectedChanged();
    void runningChanged();
    void ipAddressChanged();
    void subnetMaskChanged();

  private:
    void update();

    QTimer m_refreshTimer;
    QString m_externalInterfaceName;
    QString m_loopbackInterfaceName;
    bool m_externalInterfaceConnected;
    bool m_loopbackInterfaceConnected;
    bool m_running;
    QString m_ipAddress;
    QString m_subnetMask;
};
} // namespace Drivers::Network

#endif // DRIVERS_NETWORK_DRIVER_H
