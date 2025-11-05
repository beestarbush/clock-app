#include "DeviceConfiguration.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <algorithm>

DeviceConfiguration::DeviceConfiguration()
    : version("1.0")
{
    lastModified = QDateTime::currentDateTime();
}

QJsonObject DeviceConfiguration::toJson() const
{
    QJsonObject json;

    json["version"] = version;

    if (!deviceId.isEmpty()) {
        json["device_id"] = deviceId;
    }

    if (lastModified.isValid()) {
        json["last_modified"] = lastModified.toString(Qt::ISODate);
    }

    if (!activeAppId.isEmpty()) {
        json["active_app_id"] = activeAppId;
    }

    // Serialize system-wide configuration
    if (!systemConfiguration.isEmpty()) {
        json["system-configuration"] = systemConfiguration;
    }

    // Serialize applications array - each appConfig already has all needed fields
    // from Common::Configuration::toJson() (id, type, name, order, watchface, configuration properties)
    QJsonArray appsArray;
    for (const QJsonObject& appConfig : applications) {
        appsArray.append(appConfig);
    }
    json["applications"] = appsArray;

    return json;
}

DeviceConfiguration DeviceConfiguration::fromJson(const QJsonObject& json)
{
    DeviceConfiguration config;

    config.version = json["version"].toString("1.0");
    config.deviceId = json["device_id"].toString();
    config.activeAppId = json["active_app_id"].toString();

    QString lastModifiedStr = json["last_modified"].toString();
    if (!lastModifiedStr.isEmpty()) {
        config.lastModified = QDateTime::fromString(lastModifiedStr, Qt::ISODate);
    }

    // Deserialize system-wide configuration
    if (json.contains("system-configuration")) {
        config.systemConfiguration = json["system-configuration"].toObject();
    }

    // Deserialize applications array - store JSON objects directly
    // Each contains metadata (id, type, name, order, watchface) + configuration properties
    QJsonArray appsArray = json["applications"].toArray();
    for (const QJsonValue& value : appsArray) {
        if (value.isObject()) {
            config.applications.append(value.toObject());
        }
    }

    return config;
}

bool DeviceConfiguration::isValid() const
{
    return !version.isEmpty() && !deviceId.isEmpty();
}

QString DeviceConfiguration::getEndpoint() const
{
    if (deviceId.isEmpty()) {
        return "/api/configuration";
    }
    return QString("/api/configuration/%1").arg(deviceId);
}

QString DeviceConfiguration::getFetchEndpoint() const
{
    return getEndpoint();
}

QString DeviceConfiguration::getUpdateEndpoint() const
{
    return getEndpoint();
}

void DeviceConfiguration::saveToFile(const QString& directory) const
{
    QString filePath = getConfigurationFilePath(directory);

    // Ensure directory exists
    QDir dir;
    if (!dir.exists(directory)) {
        dir.mkpath(directory);
    }

    QJsonDocument doc(toJson());

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to save configuration to" << filePath;
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "Configuration saved to" << filePath;
}

DeviceConfiguration DeviceConfiguration::loadFromFile(const QString& directory)
{
    QString filePath = getConfigurationFilePath(directory);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to load configuration from" << filePath;
        return DeviceConfiguration();
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid JSON in configuration file" << filePath;
        return DeviceConfiguration();
    }

    DeviceConfiguration config = fromJson(doc.object());
    qDebug() << "Configuration loaded from" << filePath << "with" << config.applicationCount() << "applications";

    return config;
}

QString DeviceConfiguration::getConfigurationFilePath(const QString& directory)
{
    return QDir(directory).filePath("configuration.json");
}

int DeviceConfiguration::applicationCount() const
{
    return applications.count();
}

bool DeviceConfiguration::hasApplications() const
{
    return !applications.isEmpty();
}

QJsonObject* DeviceConfiguration::findApplication(const QString& appId)
{
    for (QJsonObject& appConfig : applications) {
        if (appConfig["id"].toString() == appId) {
            return &appConfig;
        }
    }
    return nullptr;
}

const QJsonObject* DeviceConfiguration::findApplication(const QString& appId) const
{
    for (const QJsonObject& appConfig : applications) {
        if (appConfig["id"].toString() == appId) {
            return &appConfig;
        }
    }
    return nullptr;
}

QJsonObject* DeviceConfiguration::findApplicationByType(const QString& typeName)
{
    for (QJsonObject& appConfig : applications) {
        if (appConfig["type"].toString() == typeName) {
            return &appConfig;
        }
    }
    return nullptr;
}

void DeviceConfiguration::addApplication(const QJsonObject& appConfig)
{
    applications.append(appConfig);
    lastModified = QDateTime::currentDateTime();
}

void DeviceConfiguration::removeApplication(const QString& appId)
{
    applications.erase(
        std::remove_if(applications.begin(), applications.end(), [&appId](const QJsonObject& appConfig) {
            return appConfig["id"].toString() == appId;
        }),
        applications.end());
    lastModified = QDateTime::currentDateTime();
}

void DeviceConfiguration::updateApplication(const QJsonObject& updatedConfig)
{
    QString appId = updatedConfig["id"].toString();
    for (int i = 0; i < applications.size(); ++i) {
        if (applications[i]["id"].toString() == appId) {
            applications[i] = updatedConfig;
            lastModified = QDateTime::currentDateTime();
            return;
        }
    }

    // If not found, add it
    addApplication(updatedConfig);
}

void DeviceConfiguration::sortApplicationsByOrder()
{
    std::sort(applications.begin(), applications.end(), [](const QJsonObject& a, const QJsonObject& b) {
        return a["order"].toInt() < b["order"].toInt();
    });
}
