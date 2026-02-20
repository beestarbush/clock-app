#ifndef SERVICES_WEBSOCKET_SERVICE_H
#define SERVICES_WEBSOCKET_SERVICE_H

#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QWebSocket>

#include <functional>

#include "Types.h"

namespace Drivers::Network
{
class Driver;
}

namespace Services::WebSocket
{

class Service : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString serverUrl READ serverUrl WRITE setServerUrl NOTIFY serverUrlChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)

  public:
    using ResponseCallback = std::function<void(bool success, const QJsonObject& result, const QString& error)>;

    explicit Service(Drivers::Network::Driver& network, QObject* parent = nullptr);

    QString serverUrl() const;
    bool connected() const;

    void setServerUrl(const QString& url);

    /**
     * Send a request to the server and receive a correlated response via callback.
     * The request is assigned an auto-incrementing id for correlation.
     */
    void request(const Method& method, const QJsonObject& params, ResponseCallback callback);

    /**
     * Publish a message to a topic (fire-and-forget, no response expected).
     * Used for both app→backend notifications (e.g. status.update) and
     * backend→app broadcasts (e.g. configuration).
     */
    void publish(const Topic& topic, const QJsonObject& params = QJsonObject());

    /**
     * Subscribe to a topic. The server will send publish messages for this topic.
     * Subscriptions are automatically re-established on reconnect.
     */
    void subscribe(const Topic& topic);

    /**
     * Unsubscribe from a topic.
     */
    void unsubscribe(const Topic& topic);

  signals:
    void serverUrlChanged();
    void connectedChanged();

    /**
     * Emitted when a publish message is received from the server for a subscribed topic.
     */
    void publishReceived(const Topic& topic, const QJsonObject& data);

  private:
    void loadProperties();
    void saveProperty(const QString& key, const QVariant& value);
    void connectToSocket();
    void disconnectFromSocket();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void onTextMessageReceived(const QString& message);
    void dispatchMessage(const QJsonObject& message);
    void resubscribeAll();

    Drivers::Network::Driver& m_network;
    QWebSocket m_webSocket;
    QString m_serverUrl;
    bool m_connected;
    int m_nextRequestId;
    QHash<QString, ResponseCallback> m_pendingRequests;
    QList<Topic> m_subscribedTopics;
};

} // namespace Services::WebSocket

#endif // SERVICES_WEBSOCKET_SERVICE_H
