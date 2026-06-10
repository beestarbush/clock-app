#include "Application.h"
#include "services/media/Service.h"
#include "websocket/Types.h"
#include "websocket/client/Service.h"
#include <QDebug>
#include <QJsonObject>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(NoOperationApplication, "NoOperationApplication")

using namespace Applications::NoOperation;

Application::Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media, Common::Communication::WebSocket::Client::Service& webSocket, QObject* parent)
    : Common::Application(id, type, displayName, order, watchface, parent),
      m_configuration(new Common::Configuration(id, parent)),
      m_media(media)
{
    webSocket.subscribe(Common::Communication::WebSocket::Topic::ApplicationDetail,
                        QJsonObject{{"id", id}},
                        this,
                        [this](const QJsonObject& payload) {
                            applyConfiguration(payload);
                        });

    // Refresh background when media sync completes
    connect(&m_media, &Services::Media::Service::syncCompleted, this, [this]() {
        emit m_configuration->backgroundChanged();
    });
}

Common::Configuration* Application::configuration() const
{
    return m_configuration;
}

void Application::applyConfiguration(const QJsonObject& configuration)
{
    m_configuration->fromJson(configuration);
    qCDebug(NoOperationApplication) << "Configuration: " << *m_configuration;
}
