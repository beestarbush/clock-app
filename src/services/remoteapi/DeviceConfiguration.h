#ifndef DEVICECONFIGURATION_H
#define DEVICECONFIGURATION_H

#include "SerializableObject.h"
#include <QDateTime>
#include <QJsonObject>
#include <QList>
#include <QString>

/**
 * Device Configuration domain object
 * Represents the complete configuration for the device including all applications
 * Stores configurations as JSON objects that map to Common::Configuration hierarchy
 */
class DeviceConfiguration : public SerializableObject
{
  public:
    DeviceConfiguration();

    // Metadata
    QString version;
    QDateTime lastModified;
    QString activeAppId;

    // System-wide configuration (pendulum colors, brightness, volume, etc.)
    QJsonObject systemConfiguration;

    // Applications list - stored as JSON (from Common::Configuration::toJson())
    QList<QJsonObject> applications;

    // SerializableObject interface
    QJsonObject toJson() const override;
    bool isValid() const override;
    QString typeName() const override
    {
        return "DeviceConfiguration";
    }

    // Endpoint routing
    QString getEndpoint() const override;
    QString getFetchEndpoint() const override;
    QString getUpdateEndpoint() const override;

    // Deserialization
    static DeviceConfiguration fromJson(const QJsonObject& json);

    // Local persistence (configuration.json file)
    void saveToFile(const QString& directory) const;
    static DeviceConfiguration loadFromFile(const QString& directory);
    static QString getConfigurationFilePath(const QString& directory);

    // Convenience methods
    int applicationCount() const;
    bool hasApplications() const;
    QJsonObject* findApplication(const QString& appId);
    const QJsonObject* findApplication(const QString& appId) const;
    QJsonObject* findApplicationByType(const QString& typeName);
    void addApplication(const QJsonObject& appConfig);
    void removeApplication(const QString& appId);
    void updateApplication(const QJsonObject& appConfig);
    void sortApplicationsByOrder();
};

#endif // DEVICECONFIGURATION_H
