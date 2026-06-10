#include "Service.h"
#include "websocket/client/Service.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(EnvironmentService, "EnvironmentService")

using namespace Services::Environment;

Service::Service(Common::Communication::WebSocket::Client::Service& webSocket, QObject* parent)
    : QObject(parent),
      m_webSocket(webSocket),
      m_co2PartsPerMillion(0.0),
      m_temperatureCelsius(0.0),
      m_humidityPercentage(0.0)
{
    m_webSocket.subscribe(Common::Communication::WebSocket::Topic::Environment,
                          QJsonObject(),
                          this,
                          [this](const QJsonObject& data) {
                              onEnvironmentDataReceived(data);
                          });
}

double Service::co2PartsPerMillion() const
{
    return m_co2PartsPerMillion;
}

double Service::temperatureCelsius() const
{
    return m_temperatureCelsius;
}

double Service::humidityPercentage() const
{
    return m_humidityPercentage;
}

void Service::onEnvironmentDataReceived(const QJsonObject& data)
{
    qCDebug(EnvironmentService) << "Environment data received:" << data;

    if (data.contains("co2_parts_per_million")) {
        double lValue = data["co2_parts_per_million"].toDouble();
        if (m_co2PartsPerMillion != lValue) {
            m_co2PartsPerMillion = lValue;
            emit co2PartsPerMillionChanged();
        }
    }

    if (data.contains("temperature_celsius")) {
        double lValue = data["temperature_celsius"].toDouble();
        if (m_temperatureCelsius != lValue) {
            m_temperatureCelsius = lValue;
            emit temperatureCelsiusChanged();
        }
    }

    if (data.contains("humidity_percentage")) {
        double lValue = data["humidity_percentage"].toDouble();
        if (m_humidityPercentage != lValue) {
            m_humidityPercentage = lValue;
            emit humidityPercentageChanged();
        }
    }
}
