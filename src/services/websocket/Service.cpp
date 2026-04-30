#include "Service.h"
#include "drivers/network/Driver.h"

#include <QDebug>
#include <QLoggingCategory>
#include <QSettings>
#include <QTimer>

Q_LOGGING_CATEGORY(WebSocketService, "WebSocketService")

using namespace Services::WebSocket;

const QString PROPERTIES_GROUP_NAME = QStringLiteral("websocket-api");
const QString PROPERTY_SERVER_URL_KEY = QStringLiteral("url");
const QString PROPERTY_SERVER_URL_DEFAULT = QStringLiteral("ws://127.0.0.1:5000/ws");
constexpr int RECONNECT_INTERVAL_MS = 5000;

Service::Service(Drivers::Network::Driver& network, QObject* parent)
    : QObject(parent),
      m_network(network),
      m_webSocket(QString(), QWebSocketProtocol::VersionLatest, this),
      m_serverUrl(PROPERTY_SERVER_URL_DEFAULT),
      m_connected(false),
      m_nextRequestId(1)
{
    loadProperties();

    connect(&m_webSocket, &QWebSocket::connected, this, &Service::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &Service::onDisconnected);
    connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred), this, &Service::onError);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &Service::onTextMessageReceived);

    connect(&m_network, &Drivers::Network::Driver::loopbackInterfaceConnectedChanged, this, [this]() {
        if (m_network.loopbackInterfaceConnected()) {
            connectToSocket();
        }
        else {
            disconnectFromSocket();
        }
    });

    if (m_network.loopbackInterfaceConnected()) {
        connectToSocket();
    }
    else {
        qCInfo(WebSocketService) << "Waiting for network before connecting to" << m_serverUrl;
    }
}

QString Service::serverUrl() const
{
    return m_serverUrl;
}

bool Service::connected() const
{
    return m_connected;
}

void Service::setServerUrl(const QString& url)
{
    if (m_serverUrl == url)
        return;

    m_serverUrl = url;
    saveProperty(PROPERTY_SERVER_URL_KEY, url);
    emit serverUrlChanged();

    disconnectFromSocket();
    if (m_network.loopbackInterfaceConnected()) {
        connectToSocket();
    }
}

void Service::request(const Method& method, const QJsonObject& params, ResponseCallback callback)
{
    if (!m_connected) {
        qCWarning(WebSocketService) << "Disconnected, cannot send request:" << methodToString(method);
        if (callback) {
            callback(false, QJsonObject(), QStringLiteral("Disconnected"));
        }
        return;
    }

    QString id = QString::number(m_nextRequestId++);

    // Store callback for correlation
    m_pendingRequests.insert(id, callback);

    QJsonObject req;
    req["jsonrpc"] = QStringLiteral("2.0");
    req["type"] = messageTypeToString(MessageType::Request);
    req["method"] = methodToString(method);
    req["params"] = params;
    req["id"] = id;

    QJsonDocument doc(req);
    QString message = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
    qCDebug(WebSocketService) << "-> request" << message;
    m_webSocket.sendTextMessage(message);
}

void Service::publish(const Topic& topic, const QJsonObject& params)
{
    if (!m_connected) {
        qCWarning(WebSocketService) << "Disconnected, cannot publish to topic:" << topic;
        return;
    }

    QJsonObject msg;
    msg["jsonrpc"] = QStringLiteral("2.0");
    msg["type"] = messageTypeToString(MessageType::Publish);
    msg["topic"] = topicToString(topic);
    msg["params"] = params;

    QJsonDocument doc(msg);
    QString message = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
    qCDebug(WebSocketService) << "-> publish" << message;
    m_webSocket.sendTextMessage(message);
}

void Service::subscribe(const Topic& topic)
{
    if (!m_subscribedTopics.contains(topic)) {
        m_subscribedTopics.append(topic);
    }

    if (!m_connected) {
        qCDebug(WebSocketService) << "Will subscribe to" << topic << "when connected";
        return;
    }

    QJsonObject params;
    params["topic"] = topicToString(topic);
    request(Method::Subscribe, params, [topic](bool success, const QJsonObject&, const QString& error) {
        if (success) {
            qCDebug(WebSocketService) << "Subscribed to topic:" << topic;
        }
        else {
            qCWarning(WebSocketService) << "Failed to subscribe to" << topic << ":" << error;
        }
    });
}

void Service::unsubscribe(const Topic& topic)
{
    m_subscribedTopics.removeAll(topic);

    if (!m_connected) {
        return;
    }

    QJsonObject params;
    params["topic"] = topicToString(topic);
    request(Method::Unsubscribe, params, [topic](bool success, const QJsonObject&, const QString& error) {
        if (success) {
            qCDebug(WebSocketService) << "Unsubscribed from topic:" << topic;
        }
        else {
            qCWarning(WebSocketService) << "Failed to unsubscribe from" << topic << ":" << error;
        }
    });
}

void Service::connectToSocket()
{
    if (m_serverUrl.isEmpty())
        return;
    if (m_webSocket.state() == QAbstractSocket::ConnectedState ||
        m_webSocket.state() == QAbstractSocket::ConnectingState)
        return;

    qCDebug(WebSocketService) << "Connecting to" << m_serverUrl;
    m_webSocket.open(QUrl(m_serverUrl));
}

void Service::disconnectFromSocket()
{
    m_webSocket.close();
}

void Service::onConnected()
{
    qCDebug(WebSocketService) << "Connected!";
    m_connected = true;
    emit connectedChanged();

    // Subscribe to all topics on the server
    resubscribeAll();
}

void Service::onDisconnected()
{
    qCDebug(WebSocketService) << "Disconnected, TODO: raise notification that internal communication failed.";

    // Clear pending requests — they will never get a response
    for (auto it = m_pendingRequests.begin(); it != m_pendingRequests.end(); ++it) {
        if (it.value()) {
            it.value()(false, QJsonObject(), QStringLiteral("Disconnected"));
        }
    }
    m_pendingRequests.clear();

    if (m_connected) {
        m_connected = false;
        emit connectedChanged();
    }

    if (m_network.loopbackInterfaceConnected()) {
        QTimer::singleShot(RECONNECT_INTERVAL_MS, this, &Service::connectToSocket);
    }
}

void Service::onError(QAbstractSocket::SocketError error)
{
    qCWarning(WebSocketService) << "Error:" << error << m_webSocket.errorString();
}

void Service::onTextMessageReceived(const QString& message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (doc.isObject()) {
        dispatchMessage(doc.object());
    }
    else {
        qCWarning(WebSocketService) << "Received invalid JSON:" << message;
    }
}

void Service::dispatchMessage(const QJsonObject& message)
{
    QString messageStr = QString::fromUtf8(QJsonDocument(message).toJson(QJsonDocument::Compact));
    qCDebug(WebSocketService) << "<- response/publish" << messageStr;
    MessageType type = messageTypeFromString(message["type"].toString());

    if (type == MessageType::Response) {
        // Correlated response — look up callback by id
        QString id = message["id"].toString();
        if (m_pendingRequests.contains(id)) {
            ResponseCallback callback = m_pendingRequests.take(id);
            if (message.contains("error")) {
                QString error = message["error"].toObject()["message"].toString();
                callback(false, QJsonObject(), error);
            }
            else {
                QJsonObject result = message["result"].toObject();
                callback(true, result, QString());
            }
        }
        else {
            qCDebug(WebSocketService) << "Received response for unknown request id:" << id;
        }
    }
    else if (type == MessageType::Publish) {
        // Topic-based publish from server
        Topic topic = topicFromString(message["topic"].toString());
        QJsonObject data = message["params"].toObject();
        emit publishReceived(topic, data);
    }
    else {
        qCWarning(WebSocketService) << "Unknown message type received:" << type;
    }
}

void Service::resubscribeAll()
{
    for (const Topic& topic : m_subscribedTopics) {
        QJsonObject params;
        params["topic"] = topicToString(topic);
        request(Method::Subscribe, params, [topic](bool success, const QJsonObject&, const QString& error) {
            if (success) {
                qCDebug(WebSocketService) << "Subscribed to topic:" << topicToString(topic);
            }
            else {
                qCWarning(WebSocketService) << "Failed to subscribe to" << topicToString(topic) << ":" << error;
            }
        });
    }
}

void Service::loadProperties()
{
    QSettings settings;
    settings.beginGroup(PROPERTIES_GROUP_NAME);
    m_serverUrl = settings.value(PROPERTY_SERVER_URL_KEY, PROPERTY_SERVER_URL_DEFAULT).toString();
    settings.endGroup();
}

void Service::saveProperty(const QString& key, const QVariant& value)
{
    QSettings settings;
    settings.beginGroup(PROPERTIES_GROUP_NAME);
    settings.setValue(key, value);
    settings.endGroup();
}
