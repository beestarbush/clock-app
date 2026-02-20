#ifndef SERVICES_CONFIGURATION_DEVICECONFIGURATION_H
#define SERVICES_CONFIGURATION_DEVICECONFIGURATION_H

#include <QDateTime>
#include <QJsonObject>
#include <QList>
#include <QString>

namespace Services::Configuration
{

/**
 * Device Configuration domain object
 * Represents the complete configuration for the device including all applications
 * Stores configurations as JSON objects that map to Common::Configuration hierarchy
 */
class DeviceConfiguration
{
  public:
    DeviceConfiguration();

    // Metadata
    QString version;
    QDateTime lastModified;
    QString deviceId; 
    QString activeAppId;

    // System-wide configuration (pendulum colors, brightness, volume, etc.)
    QJsonObject systemConfiguration;

    // Applications list - stored as JSON (from Common::Configuration::toJson())
    // Each object must have an "id" and "type" field
    QList<QJsonObject> applications;

    // Serialization
    QJsonObject toJson() const;
    static DeviceConfiguration fromJson(const QJsonObject& json);
    bool isValid() const;

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

} // namespace Services::Configuration

#endif // SERVICES_CONFIGURATION_DEVICECONFIGURATION_H
