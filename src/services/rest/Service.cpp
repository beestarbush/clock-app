#include "Service.h"
#include "drivers/network/Driver.h"

#include <QDebug>
#include <QSettings>
#include <QTimer>

using namespace Services::Rest;

constexpr int NETWORK_TIMEOUT_MS = 30 * 1000; // 30 seconds
const QString PROPERTIES_GROUP_NAME = QStringLiteral("rest-api");
const QString PROPERTY_SERVER_URL_KEY = QStringLiteral("url");
const QString PROPERTY_SERVER_URL_DEFAULT = QStringLiteral("http://127.0.0.1:5000");

Service::Service(Drivers::Network::Driver& network, QObject* parent)
    : QObject(parent),
      m_network(network),
      m_networkManager(this),
      m_serverUrl(PROPERTY_SERVER_URL_DEFAULT)
{
    loadProperties();
}

QString Service::serverUrl() const
{
    return m_serverUrl;
}

void Service::setServerUrl(const QString& url)
{
    if (m_serverUrl == url)
        return;

    m_serverUrl = url;
    saveProperty(PROPERTY_SERVER_URL_KEY, url);
    emit serverUrlChanged();
}

void Service::get(const QString& endpoint, ResponseCallback callback)
{
    if (!m_network.loopbackInterfaceConnected()) {
        qWarning() << "REST: Network not connected, cannot GET" << endpoint;
        if (callback) QTimer::singleShot(0, this, [callback]() { callback(false, QJsonObject(), "Network not connected"); });
        return;
    }

    QNetworkRequest request = createRequest(endpoint);
    QNetworkReply* reply = m_networkManager.get(request);

    PendingRequest pending;
    pending.reply = reply;
    pending.responseCallback = callback;
    pending.isBinaryDownload = false;
    m_pendingRequests[reply] = pending;

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleResponse(reply);
    });
}

void Service::post(const QString& endpoint, const QJsonObject& payload, ResponseCallback callback)
{
    if (!m_network.loopbackInterfaceConnected()) {
        qWarning() << "REST: Network not connected, cannot POST" << endpoint;
        if (callback) QTimer::singleShot(0, this, [callback]() { callback(false, QJsonObject(), "Network not connected"); });
        return;
    }

    QNetworkRequest request = createRequest(endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(payload);
    QNetworkReply* reply = m_networkManager.post(request, doc.toJson());

    PendingRequest pending;
    pending.reply = reply;
    pending.responseCallback = callback;
    pending.isBinaryDownload = false;
    m_pendingRequests[reply] = pending;

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleResponse(reply);
    });
}

void Service::put(const QString& endpoint, const QJsonObject& payload, ResponseCallback callback)
{
    if (!m_network.loopbackInterfaceConnected()) {
        qWarning() << "REST: Network not connected, cannot PUT" << endpoint;
        if (callback) QTimer::singleShot(0, this, [callback]() { callback(false, QJsonObject(), "Network not connected"); });
        return;
    }

    QNetworkRequest request = createRequest(endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(payload);
    QNetworkReply* reply = m_networkManager.put(request, doc.toJson());

    PendingRequest pending;
    pending.reply = reply;
    pending.responseCallback = callback;
    pending.isBinaryDownload = false;
    m_pendingRequests[reply] = pending;

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleResponse(reply);
    });
}

void Service::deleteResource(const QString& endpoint, ResponseCallback callback)
{
    if (!m_network.loopbackInterfaceConnected()) {
        qWarning() << "REST: Network not connected, cannot DELETE" << endpoint;
        if (callback) QTimer::singleShot(0, this, [callback]() { callback(false, QJsonObject(), "Network not connected"); });
        return;
    }

    QNetworkRequest request = createRequest(endpoint);
    QNetworkReply* reply = m_networkManager.deleteResource(request);

    PendingRequest pending;
    pending.reply = reply;
    pending.responseCallback = callback;
    pending.isBinaryDownload = false;
    m_pendingRequests[reply] = pending;

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleResponse(reply);
    }); 
}

void Service::download(const QString& endpoint, DataCallback callback)
{
    if (!m_network.loopbackInterfaceConnected()) {
        qWarning() << "REST: Network not connected, cannot download" << endpoint;
        if (callback) QTimer::singleShot(0, this, [callback]() { callback(false, QByteArray(), "Network not connected"); });
        return;
    }

    QNetworkRequest request = createRequest(endpoint);
    QNetworkReply* reply = m_networkManager.get(request);

    PendingRequest pending;
    pending.reply = reply;
    pending.dataCallback = callback;
    pending.isBinaryDownload = true;
    m_pendingRequests[reply] = pending;

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleResponse(reply);
    });
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

QNetworkRequest Service::createRequest(const QString& endpoint)
{
    QString urlStr = m_serverUrl;
    if (!urlStr.endsWith('/') && !endpoint.startsWith('/')) {
        urlStr += '/';
    }
    urlStr += endpoint;

    QNetworkRequest request((QUrl(urlStr)));
    request.setTransferTimeout(NETWORK_TIMEOUT_MS);
    return request;
}

void Service::handleResponse(QNetworkReply* reply)
{
    reply->deleteLater();
    if (!m_pendingRequests.contains(reply)) return;

    PendingRequest pending = m_pendingRequests.take(reply);

    if (reply->error() != QNetworkReply::NoError) {
        handleError(reply, reply->errorString());
        // Propagate error via callback
        if (pending.isBinaryDownload && pending.dataCallback) {
            pending.dataCallback(false, QByteArray(), reply->errorString());
        } else if (pending.responseCallback) {
            pending.responseCallback(false, QJsonObject(), reply->errorString());
        }
        return;
    }

    QByteArray data = reply->readAll();

    if (pending.isBinaryDownload) {
        if (pending.dataCallback) {
            pending.dataCallback(true, data, QString());
        }
    } else {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            if (pending.responseCallback) {
                pending.responseCallback(true, doc.object(), QString());
            }
        } else {
             QString err = "Invalid JSON response";
             if (pending.responseCallback) {
                pending.responseCallback(false, QJsonObject(), err);
            }
        }
    }
}

void Service::handleError(QNetworkReply* reply, const QString& errorString)
{
    qWarning() << "REST Error:" << errorString << "URL:" << reply->url().toString();
}
