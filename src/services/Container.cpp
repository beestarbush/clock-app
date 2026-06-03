#include "Container.h"
#include "audio/Service.h"
#include "datetime/Service.h"
#include "drivers/Container.h"

#include <QLoggingCategory>
#include <QSettings>
#include <QTimer>

using namespace Services;

Q_LOGGING_CATEGORY(ServiceContainer, "ServiceContainer")

const QString WEBSOCKET_PROPERTIES_GROUP_NAME = QStringLiteral("websocket-api");
const QString WEBSOCKET_PROPERTY_SERVER_URL_KEY = QStringLiteral("url");
const QString WEBSOCKET_PROPERTY_SERVER_URL_DEFAULT = QStringLiteral("ws://127.0.0.1:5000/ws");

const QString REST_PROPERTIES_GROUP_NAME = QStringLiteral("rest-api");
const QString REST_PROPERTY_SERVER_URL_KEY = QStringLiteral("url");
const QString REST_PROPERTY_SERVER_URL_DEFAULT = QStringLiteral("http://127.0.0.1:5000");

constexpr int WEBSOCKET_RECONNECT_INTERVAL_MS = 5000;

Container::Container(Drivers::Container& drivers, QObject* parent)
    : QObject(parent),
      m_version(new Version::Service(this)),
      m_logging(new Logging::Service(this)),
      m_rest(new Common::Communication::Rest::Client::Service(this)),
      m_websocket(new Common::Communication::WebSocket::Client::Service(this)),
      m_audio(new Audio::Service(*m_websocket, this)),
      m_environment(new Environment::Service(*m_websocket, this)),
      m_notification(new Notification::Service(this)),
      m_media(new Media::Service(*m_websocket, *m_rest, this)),
      m_systemMonitor(new SystemMonitor::Service(*m_websocket, *m_version, *m_notification, this)),
      m_configuration(new Configuration::Service(*m_websocket, this)),
      m_dateTime(new DateTime::Service(this)),
      m_qmlInterface(new QmlInterface::Service(this))
{
    QSettings settings;

    settings.beginGroup(WEBSOCKET_PROPERTIES_GROUP_NAME);
    m_websocket->setServerUrl(settings.value(WEBSOCKET_PROPERTY_SERVER_URL_KEY, WEBSOCKET_PROPERTY_SERVER_URL_DEFAULT).toString());
    settings.endGroup();

    settings.beginGroup(REST_PROPERTIES_GROUP_NAME);
    m_rest->setServerUrl(settings.value(REST_PROPERTY_SERVER_URL_KEY, REST_PROPERTY_SERVER_URL_DEFAULT).toString());
    settings.endGroup();

    connect(drivers.m_network,
            &Drivers::Network::Driver::loopbackInterfaceConnectedChanged,
            this,
            [this, &drivers]() {
                if (drivers.m_network->loopbackInterfaceConnected()) {
                    m_websocket->connectToSocket();
                }
                else {
                    m_websocket->disconnectFromSocket();
                }
            });

    connect(m_websocket,
            &Common::Communication::WebSocket::Client::Service::connectedChanged,
            this,
            [this, &drivers]() {
                if (m_websocket->connected() || !drivers.m_network->loopbackInterfaceConnected()) {
                    return;
                }

                QTimer::singleShot(WEBSOCKET_RECONNECT_INTERVAL_MS, this, [this, &drivers]() {
                    if (!m_websocket->connected() && drivers.m_network->loopbackInterfaceConnected()) {
                        qCDebug(ServiceContainer) << "Reconnecting websocket client";
                        m_websocket->connectToSocket();
                    }
                });
            });

    if (drivers.m_network->loopbackInterfaceConnected()) {
        m_websocket->connectToSocket();
    }
    else {
        qCInfo(ServiceContainer) << "Waiting for loopback network before websocket connect";
    }
}

QmlInterface::Service* Container::qmlInterface() const
{
    return m_qmlInterface;
}
