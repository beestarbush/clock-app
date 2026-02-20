#include "Container.h"
#include "services/Container.h"
#include "services/configuration/DeviceConfiguration.h"
#include <QDebug>

using namespace Applications;

Container::Container(Services::Container& services, QObject* parent)
    : QObject(parent),
      m_applications(),
      m_setup(new Setup::Application(m_applications, *services.m_configuration, this)),
      m_debug(new Debug::Application(this)),
      m_menu(new Menu::Application(this)),
      m_watchface(new Watchface::Application(m_applications, this))
{
    connect(services.m_configuration, &Services::Configuration::Service::configurationChanged, this, [this, &services]() {
        qInfo() << "Configuration changed, reloading applications";
        reload(*services.m_configuration, *services.m_media);
    });

    // When startup check is already completed, reload immediately to apply initial configuration (if any present).
    if (!services.m_configuration->startupCheckInProgress() &&
        !services.m_media->startupCheckInProgress()) {
        reload(*services.m_configuration, *services.m_media);
    }

    auto notification = services.m_notification;
    notification->showInfo("System started", "The system is ready to use.", false);
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

void Container::reload(Services::Configuration::Service& configuration, Services::Media::Service& media)
{
    setReloading(true);

    Services::Configuration::DeviceConfiguration* config = configuration.getCurrentConfiguration();
    if (!config || !config->isValid() || !config->hasApplications()) {
        qWarning() << "Invalid configuration retrieved.";
        setReloading(false);
        return;
    }

    if (config->systemConfiguration.isEmpty()) {
        qWarning() << "System configuration is empty.";
        setReloading(false);
        return;
    }

    qInfo() << "Applying system configuration from device configuration";
    m_setup->applySystemConfiguration(config->systemConfiguration);

    // Destroy existing dynamic applications
    qInfo() << "Destroying existing applications before reloading from configuration";
    for (auto it = m_applications.begin(); it != m_applications.end(); ++it) {
        if (it.value()) {
            qDebug() << "Destroying application:" << it.key();
            delete it.value();
        }
    }
    m_applications.clear();

    qInfo() << "Creating" << config->applicationCount() << "applications from configuration";
    for (const QJsonObject& appConfig : config->applications) {
        QString id = appConfig["id"].toString();
        Common::Type type = Common::typeFromString(appConfig["type"].toString());
        QString displayName = appConfig["name"].toString();
        int order = appConfig["order"].toInt();
        Common::Watchface watchface = Common::watchfaceFromString(appConfig["watchface"].toString());

        if (id.isEmpty() ||
            type == Common::Type::Unknown ||
            displayName.isEmpty() ||
            watchface == Common::Watchface::None) {
            qWarning() << "Skipping creation of application with invalid metadata:" << appConfig;
            continue;
        }

        // Create application
        Common::Application* app = createApplication(id, type, displayName, order, watchface, media);
        if (app) {
            app->applyConfiguration(appConfig);
            m_applications[id] = app;
        }
        else {
            qWarning() << "Failed to create application:" << id << "of type:" << type;
        }
    }

    m_watchface->refresh();

    setReloading(false);
}

Common::Application* Container::createApplication(const QString& id, const Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media)
{
    if (type == Common::Type::Clock) {
        return new Clock::Application(id, type, displayName, order, watchface, media, this);
    }
    else if (type == Common::Type::TimeElapsed) {
        return new TimeElapsed::Application(id, type, displayName, order, watchface, media, this);
    }
    else if (type == Common::Type::Countdown) {
        return new Countdown::Application(id, type, displayName, order, watchface, media, this);
    }
    else {
        qWarning() << "Unknown application type:" << type;
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
