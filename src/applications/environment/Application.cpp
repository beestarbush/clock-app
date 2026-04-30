#include "Application.h"
#include "services/environment/Service.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(EnvironmentApplication, "EnvironmentApplication")

using namespace Applications::Environment;

Application::Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Environment::Service& environment, QObject* parent)
    : Common::Application(id, type, displayName, order, watchface, parent),
      m_configuration(new Configuration(id, this)),
      m_environment(environment)
{
    connect(&m_environment, &Services::Environment::Service::co2PartsPerMillionChanged, this, &Application::co2PartsPerMillionChanged);
    connect(&m_environment, &Services::Environment::Service::temperatureCelsiusChanged, this, &Application::temperatureCelsiusChanged);
    connect(&m_environment, &Services::Environment::Service::humidityPercentageChanged, this, &Application::humidityPercentageChanged);
}

Configuration* Application::configuration() const
{
    return m_configuration;
}

void Application::applyConfiguration(const Configuration& configuration)
{
    *m_configuration = configuration;
    qCDebug(EnvironmentApplication) << "Configuration: " << *m_configuration;
}

void Application::applyConfiguration(const QJsonObject& configuration)
{
    m_configuration->fromJson(configuration);
    qCDebug(EnvironmentApplication) << "Configuration: " << *m_configuration;
}

double Application::co2PartsPerMillion() const
{
    return m_environment.co2PartsPerMillion();
}

double Application::temperatureCelsius() const
{
    return m_environment.temperatureCelsius();
}

double Application::humidityPercentage() const
{
    return m_environment.humidityPercentage();
}
