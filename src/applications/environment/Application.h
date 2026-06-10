#ifndef ENVIRONMENT_APPLICATION_H
#define ENVIRONMENT_APPLICATION_H

#include "Configuration.h"
#include "applications/common/Application.h"
#include <QObject>

namespace Services::Environment
{
class Service;
}

namespace Common::Communication::WebSocket::Client
{
class Service;
}

namespace Applications::Environment
{
class Application : public Common::Application
{
    Q_OBJECT
    Q_PROPERTY(Configuration* configuration READ configuration CONSTANT)
    Q_PROPERTY(double co2PartsPerMillion READ co2PartsPerMillion NOTIFY co2PartsPerMillionChanged)
    Q_PROPERTY(double temperatureCelsius READ temperatureCelsius NOTIFY temperatureCelsiusChanged)
    Q_PROPERTY(double humidityPercentage READ humidityPercentage NOTIFY humidityPercentageChanged)

  public:
    Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Environment::Service& environment, Common::Communication::WebSocket::Client::Service& webSocket, QObject* parent = nullptr);

    Configuration* configuration() const override;

    void applyConfiguration(const Configuration& configuration);
    void applyConfiguration(const QJsonObject& configuration) override;

    double co2PartsPerMillion() const;
    double temperatureCelsius() const;
    double humidityPercentage() const;

  signals:
    void co2PartsPerMillionChanged();
    void temperatureCelsiusChanged();
    void humidityPercentageChanged();

  private:
    Configuration* m_configuration;
    Services::Environment::Service& m_environment;
};
} // namespace Applications::Environment

#endif // ENVIRONMENT_APPLICATION_H
