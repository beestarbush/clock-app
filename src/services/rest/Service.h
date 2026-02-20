#ifndef SERVICES_REST_SERVICE_H
#define SERVICES_REST_SERVICE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <functional>

namespace Drivers::Network
{
class Driver;
}

namespace Services::Rest
{

class Service : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString serverUrl READ serverUrl WRITE setServerUrl NOTIFY serverUrlChanged)

  public:
    explicit Service(Drivers::Network::Driver& network, QObject* parent = nullptr);

    // Properties
    QString serverUrl() const;
    void setServerUrl(const QString& url);

    // Callbacks
    using ResponseCallback = std::function<void(bool success, const QJsonObject& response, const QString& error)>;
    using DataCallback = std::function<void(bool success, const QByteArray& data, const QString& error)>;

    // HTTP Methods
    void get(const QString& endpoint, ResponseCallback callback);
    void post(const QString& endpoint, const QJsonObject& payload, ResponseCallback callback);
    void put(const QString& endpoint, const QJsonObject& payload, ResponseCallback callback);
    void deleteResource(const QString& endpoint, ResponseCallback callback);
    void download(const QString& endpoint, DataCallback callback);

  signals:
    void serverUrlChanged();

  private:
    struct PendingRequest
    {
        QNetworkReply* reply;
        ResponseCallback responseCallback;
        DataCallback dataCallback;
        bool isBinaryDownload;
    };

    void loadProperties();
    void saveProperty(const QString& key, const QVariant& value);
    QNetworkRequest createRequest(const QString& endpoint);
    void handleResponse(QNetworkReply* reply);
    void handleError(QNetworkReply* reply, const QString& errorString);

    Drivers::Network::Driver& m_network;
    QNetworkAccessManager m_networkManager;
    QMap<QNetworkReply*, PendingRequest> m_pendingRequests;

    QString m_serverUrl;
};

} // namespace Services::Rest

#endif // SERVICES_REST_SERVICE_H
