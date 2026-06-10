#include "Container.h"
#include "services/Container.h"
#include "services/environment/Service.h"
#include <QDebug>
#include <QLoggingCategory>
#include <QSet>

Q_LOGGING_CATEGORY(ApplicationContainer, "ApplicationContainer")

using namespace Applications;

Container::Container(Services::Container& services, QObject* parent)
    : QObject(parent),
      m_applications(),
      m_setup(new Setup::Application(m_applications, *services.m_configuration, *services.m_websocket, this)),
      m_debug(new Debug::Application(this)),
      m_menu(new Menu::Application(*services.m_websocket, this)),
      m_watchface(new Watchface::Application(m_applications, this))
{
    services.m_websocket->subscribe(Common::Communication::WebSocket::Topic::ApplicationList,
                                    QJsonObject(),
                                    this,
                                    [this, &services](const QJsonObject& payload) {
                                        const QJsonArray applications = payload.value("applications").toArray();
                                        qCInfo(ApplicationContainer) << "Application list changed, applying incremental update";
                                        applyApplicationList(applications, *services.m_websocket, *services.m_media, *services.m_audio, *services.m_dateTime, *services.m_environment);
                                    });

    connect(services.m_configuration, &Services::Configuration::Service::systemConfigChanged, this, [this](const QString& deviceId, const QJsonObject& systemConfig) {
        applySystemConfiguration(deviceId, systemConfig);
    });

    auto notification = services.m_notification;
    notification->showInfo("Clock started", "The clock is ready to use.", false);
}

bool Container::reloading() const
{
    return m_reloading;
}

void Container::setReloading(bool reloading)
{
    if (m_reloading != reloading) {
        m_reloading = reloading;
        emit reloadingChanged();
    }
}

void Container::applySystemConfiguration(const QString& deviceId, const QJsonObject& systemConfig)
{
    m_setup->applyDeviceConfiguration(deviceId, systemConfig);
}

void Container::applyApplicationList(const QJsonArray& applications,
                                     Common::Communication::WebSocket::Client::Service& webSocket,
                                     Services::Media::Service& media,
                                     Services::Audio::Service& audio,
                                     Services::DateTime::Service& dateTime,
                                     Services::Environment::Service& environment)
{
    setReloading(true);

    QSet<QString> incomingIds;
    for (const QJsonValue& value : applications) {
        if (!value.isObject()) {
            continue;
        }

        const QJsonObject appConfig = value.toObject();
        const QString id = appConfig.value("id").toString();
        const Common::Type type = Common::typeFromString(appConfig.value("type").toString());
        const QString displayName = appConfig.value("name").toString();
        const int order = appConfig.value("order").toInt();
        const Common::Watchface watchface = Common::watchfaceFromString(appConfig.value("watchface").toString());

        if (id.isEmpty() ||
            type == Common::Type::Unknown ||
            displayName.isEmpty() ||
            watchface == Common::Watchface::None) {
            qCWarning(ApplicationContainer) << "Skipping application with invalid metadata:" << appConfig;
            continue;
        }

        incomingIds.insert(id);

        if (!m_applications.contains(id)) {
            Common::Application* app = createApplication(id, type, displayName, order, watchface, webSocket, media, audio, dateTime, environment);
            if (!app) {
                qCWarning(ApplicationContainer) << "Failed to create application:" << id << "of type:" << type;
                continue;
            }

            m_applications[id] = app;
        }
    }

    const QList<QString> existingIds = m_applications.keys();
    for (const QString& id : existingIds) {
        if (incomingIds.contains(id)) {
            continue;
        }

        Common::Application* app = m_applications.take(id);
        if (app) {
            qCDebug(ApplicationContainer) << "Removing application:" << id;
            delete app;
        }
    }

    m_watchface->refresh();
    setReloading(false);
}

Common::Application* Container::createApplication(const QString& id, const Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Common::Communication::WebSocket::Client::Service& webSocket, Services::Media::Service& media, Services::Audio::Service& audio, Services::DateTime::Service& dateTime, Services::Environment::Service& environment)
{
    if (type == Common::Type::Clock) {
        return new Clock::Application(id, type, displayName, order, watchface, media, audio, webSocket, this);
    }
    else if (type == Common::Type::TimeElapsed) {
        return new TimeElapsed::Application(id, type, displayName, order, watchface, media, audio, webSocket, this);
    }
    else if (type == Common::Type::Countdown) {
        return new Countdown::Application(id, type, displayName, order, watchface, media, audio, webSocket, this);
    }
    else if (type == Common::Type::NoOperation) {
        return new NoOperation::Application(id, type, displayName, order, watchface, media, webSocket, this);
    }
    else if (type == Common::Type::CurrentDate) {
        return new CurrentDate::Application(id, type, displayName, order, watchface, media, dateTime, webSocket, this);
    }
    else if (type == Common::Type::Environment) {
        return new Environment::Application(id, type, displayName, order, watchface, environment, webSocket, this);
    }
    else {
        qCWarning(ApplicationContainer) << "Unknown application type:" << type;
        return nullptr;
    }
}

Common::Application* Container::application(const QString& id) const
{
    return m_applications.value(id, nullptr);
}

QList<QString> Container::applicationIds() const
{
    return m_applications.keys();
}
