#ifndef APPS_NOOPERATION_H
#define APPS_NOOPERATION_H

#include "applications/common/Application.h"
#include "applications/common/Configuration.h"
#include <QObject>

namespace Services::Media
{
class Service;
}

namespace Common::Communication::WebSocket::Client
{
class Service;
}

namespace Applications::NoOperation
{
class Application : public Common::Application
{
    Q_OBJECT
    Q_PROPERTY(Common::Configuration* configuration READ configuration CONSTANT)

  public:
    Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media, Common::Communication::WebSocket::Client::Service& webSocket, QObject* parent = nullptr);

    Common::Configuration* configuration() const override;

    void applyConfiguration(const QJsonObject& configuration) override;

  private:
    Common::Configuration* m_configuration;
    Services::Media::Service& m_media;
};
} // namespace Applications::NoOperation

#endif // APPS_NOOPERATION_H
