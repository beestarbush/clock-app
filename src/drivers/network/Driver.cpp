#include "Driver.h"
#include <QDebug>
#include <QNetworkInterface>
using namespace Drivers::Network;

#ifdef PLATFORM_IS_TARGET
const QString EXTERNAL_INTERFACE_NAME = QStringLiteral("wlan0");
#else
const QString EXTERNAL_INTERFACE_NAME = QStringLiteral("eth0");
#endif
const QString LOOPBACK_INTERFACE_NAME = QStringLiteral("lo");
constexpr quint16 REFRESH_INTERVAL_MS = 30 * 1000; // Refresh every 30 seconds

Driver::Driver(QObject* parent)
    : QObject(parent),
      m_refreshTimer(this),
      m_externalInterfaceName(EXTERNAL_INTERFACE_NAME),
      m_loopbackInterfaceName(LOOPBACK_INTERFACE_NAME),
      m_externalInterfaceConnected(false),
      m_loopbackInterfaceConnected(false),
      m_running(false),
      m_ipAddress(""),
      m_subnetMask("")
{
    update();

    connect(&m_refreshTimer, &QTimer::timeout, this, &Driver::update);
    m_refreshTimer.start(REFRESH_INTERVAL_MS);
}

QString Driver::externalInterfaceName() const
{
    return m_externalInterfaceName;
}

QString Driver::loopbackInterfaceName() const
{
    return m_loopbackInterfaceName;
}

bool Driver::externalInterfaceConnected() const
{
    return m_externalInterfaceConnected;
}

bool Driver::loopbackInterfaceConnected() const
{
    return m_loopbackInterfaceConnected;
}

bool Driver::running() const
{
    return m_running;
}

QString Driver::ipAddress() const
{
    return m_ipAddress;
}

QString Driver::subnetMask() const
{
    return m_subnetMask;
}

void Driver::update()
{
    for (const QNetworkInterface& interface : QNetworkInterface::allInterfaces()) {
        if (interface.name() == m_externalInterfaceName) {
            bool wasConnected = m_externalInterfaceConnected;
            bool wasRunning = m_running;

            m_externalInterfaceConnected = interface.flags() & QNetworkInterface::IsUp;
            m_running = interface.flags() & QNetworkInterface::IsRunning;

            if (m_externalInterfaceConnected != wasConnected) {
                emit externalInterfaceConnectedChanged();
            }
            if (m_running != wasRunning) {
                emit runningChanged();
            }

            auto entries = interface.addressEntries();
            if (!entries.isEmpty()) {
                QString newIpAddress = entries.first().ip().toString();
                QString newSubnetMask = entries.first().netmask().toString();

                if (m_ipAddress != newIpAddress) {
                    m_ipAddress = newIpAddress;
                    emit ipAddressChanged();
                }
                if (m_subnetMask != newSubnetMask) {
                    m_subnetMask = newSubnetMask;
                    emit subnetMaskChanged();
                }
            }
            else {
                if (!m_ipAddress.isEmpty()) {
                    m_ipAddress.clear();
                    emit ipAddressChanged();
                }
                if (!m_subnetMask.isEmpty()) {
                    m_subnetMask.clear();
                    emit subnetMaskChanged();
                }
            }
        }

        if (interface.name() == m_loopbackInterfaceName) {
            bool wasConnected = m_loopbackInterfaceConnected;
            m_loopbackInterfaceConnected = interface.flags() & QNetworkInterface::IsUp;

            if (m_loopbackInterfaceConnected != wasConnected) {
                emit loopbackInterfaceConnectedChanged();
            }
        }
    }
}
