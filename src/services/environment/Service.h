#ifndef SERVICES_ENVIRONMENT_SERVICE_H
#define SERVICES_ENVIRONMENT_SERVICE_H

#include "websocket/Types.h"
#include "websocket/client/Service.h"
#include <QJsonObject>
#include <QObject>

namespace Services::Environment
{
class Service : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double co2PartsPerMillion READ co2PartsPerMillion NOTIFY co2PartsPerMillionChanged)
    Q_PROPERTY(double temperatureCelsius READ temperatureCelsius NOTIFY temperatureCelsiusChanged)
    Q_PROPERTY(double humidityPercentage READ humidityPercentage NOTIFY humidityPercentageChanged)

  public:
    explicit Service(Common::Communication::WebSocket::Client::Service& webSocket, QObject* parent = nullptr);

    double co2PartsPerMillion() const;
    double temperatureCelsius() const;
    double humidityPercentage() const;

  signals:
    void co2PartsPerMillionChanged();
    void temperatureCelsiusChanged();
    void humidityPercentageChanged();

  private:
    void onEnvironmentDataReceived(const QJsonObject& data);

    Common::Communication::WebSocket::Client::Service& m_webSocket;
    double m_co2PartsPerMillion;
    double m_temperatureCelsius;
    double m_humidityPercentage;
};
} // namespace Services::Environment

#endif // SERVICES_ENVIRONMENT_SERVICE_H
