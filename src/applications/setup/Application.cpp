#include "Application.h"
#include "applications/common/Application.h"
#include "applications/common/Configuration.h"
#include "services/configuration/Service.h"
#include "services/remoteapi/DeviceStatus.h"
#include "services/remoteapi/Service.h"
#include <QDate>
#include <QDebug>
#include <QJsonObject>
#include <QSettings>
using namespace Applications::Setup;

const QString PROPERTIES_GROUP_NAME = QStringLiteral("setup");
const QString PROPERTY_SETUP_COMPLETE_KEY = QStringLiteral("setup-complete");
const bool PROPERTY_SETUP_COMPLETE_DEFAULT = false;
const QString PROPERTY_PENDULUM_BOB_COLOR_KEY = QStringLiteral("pendulum-bob-color");
const QColor PROPERTY_PENDULUM_BOB_COLOR_DEFAULT = QColor("#009950");
const QString PROPERTY_PENDULUM_ROD_COLOR_KEY = QStringLiteral("pendulum-rod-color");
const QColor PROPERTY_PENDULUM_ROD_COLOR_DEFAULT = QColor("#333333");
const QString PROPERTY_PENDULUM_BACKGROUND_COLOR_KEY = QStringLiteral("pendulum-background-color");
const QColor PROPERTY_PENDULUM_BACKGROUND_COLOR_DEFAULT = QColor("#bbbbbb");
const QString PROPERTY_BASE_COLOR_KEY = QStringLiteral("base-color");
const QColor PROPERTY_BASE_COLOR_DEFAULT = QColor("#000000");
const QString PROPERTY_ACCENT_COLOR_KEY = QStringLiteral("accent-color");
const QColor PROPERTY_ACCENT_COLOR_DEFAULT = QColor("#02996c");

Application::Application(Common::DynamicApplicationMap& applications,
                         Services::RemoteApi::Service& remoteApi,
                         Services::Configuration::Service& configurationService,
                         QObject* parent)
    : QObject(parent),
      m_setupComplete(PROPERTY_SETUP_COMPLETE_DEFAULT),
      m_currentPanel(Welcome),
      m_dialWheel(),
      m_mediaSelection(),
      m_colorSelection(),
      m_applications(applications),
      m_currentAppIndex(0),
      m_remoteApi(remoteApi),
      m_configurationService(configurationService),
      m_pendulumBobColor(PROPERTY_PENDULUM_BOB_COLOR_DEFAULT),
      m_pendulumRodColor(PROPERTY_PENDULUM_ROD_COLOR_DEFAULT),
      m_pendulumBackgroundColor(PROPERTY_PENDULUM_BACKGROUND_COLOR_DEFAULT),
      m_baseColor(PROPERTY_BASE_COLOR_DEFAULT),
      m_accentColor(PROPERTY_ACCENT_COLOR_DEFAULT)
{
    loadProperties();
}

bool Application::isSetupComplete() const
{
    return m_setupComplete;
}

Application::PanelType Application::currentPanel() const
{
    return m_currentPanel;
}

Common::Application* Application::currentApp() const
{
    auto apps = m_applications.values();
    if (m_currentAppIndex >= 0 && m_currentAppIndex < apps.size()) {
        return apps[m_currentAppIndex];
    }
    return nullptr;
}

int Application::currentAppIndex() const
{
    return m_currentAppIndex;
}

int Application::appCount() const
{
    return m_applications.size();
}

DialWheelParams Application::dialWheel() const
{
    return m_dialWheel;
}

MediaSelectionParams Application::mediaSelection() const
{
    return m_mediaSelection;
}

ColorSelectionParams Application::colorSelection() const
{
    return m_colorSelection;
}

void Application::finish()
{
    m_setupComplete = true;
    m_currentPanel = Welcome;
    m_currentAppIndex = 0;

    // Hide any open dialogs
    m_dialWheel.visible = false;
    m_mediaSelection.visible = false;
    m_colorSelection.visible = false;

    saveProperty(PROPERTY_SETUP_COMPLETE_KEY, true);

    // Register device with server when setup is complete
    registerDevice();

    emit dialWheelChanged();
    emit mediaSelectionChanged();
    emit colorSelectionChanged();
    emit currentPanelChanged();
    emit currentAppChanged();
    emit setupCompleteChanged();

    // Persist the current configuration state and trigger reload
    m_configurationService.save(buildSystemConfiguration(), m_applications);
}

void Application::next()
{
    // Hide current dialogs
    if (m_dialWheel.visible) {
        m_dialWheel.visible = false;
        emit dialWheelChanged();
    }
    if (m_mediaSelection.visible) {
        m_mediaSelection.visible = false;
        emit mediaSelectionChanged();
    }
    if (m_colorSelection.visible) {
        m_colorSelection.visible = false;
        emit colorSelectionChanged();
    }

    // Determine next panel
    PanelType nextPanel = getNextPanel(m_currentPanel);

    // If transitioning from an app phase to AppEnable, advance to next app
    bool appAdvanced = false;
    if (nextPanel == AppEnable && m_currentPanel != ServerConnection) {
        appAdvanced = advanceToNextApp();
        if (!appAdvanced) {
            nextPanel = Finish;
        }
    }

    if (nextPanel != m_currentPanel || appAdvanced) {
        m_currentPanel = nextPanel;

        // Auto-show dialogs for certain panels
        if (m_currentPanel == AppBackground) {
            m_mediaSelection.visible = true;
            emit mediaSelectionChanged();
        }
        else if (m_currentPanel == AppOpacity) {
            auto* app = currentApp();
            if (app && app->configuration()) {
                int opacityPercent = static_cast<int>(app->configuration()->backgroundOpacity() * 100);
                m_dialWheel.min = 0;
                m_dialWheel.max = 100;
                m_dialWheel.step = 1;
                m_dialWheel.value = opacityPercent;
                m_dialWheel.visible = true;
                emit dialWheelChanged();
            }
        }
        else if (m_currentPanel == AppBaseColor) {
            auto* app = currentApp();
            if (app && app->configuration()) {
                m_colorSelection.startColor = app->configuration()->baseColor();
                m_colorSelection.visible = true;
                emit colorSelectionChanged();
            }
        }
        else if (m_currentPanel == AppAccentColor) {
            auto* app = currentApp();
            if (app && app->configuration()) {
                m_colorSelection.startColor = app->configuration()->accentColor();
                m_colorSelection.visible = true;
                emit colorSelectionChanged();
            }
        }

        emit currentPanelChanged();
        emit currentAppChanged();
    }
}

void Application::reset()
{
    m_setupComplete = false;
    m_currentPanel = Welcome;
    m_currentAppIndex = 0;

    saveProperty(PROPERTY_SETUP_COMPLETE_KEY, false);

    emit currentPanelChanged();
    emit currentAppChanged();
    emit setupCompleteChanged();
}

void Application::loadProperties()
{
    static QSettings settings;
    settings.beginGroup(PROPERTIES_GROUP_NAME);
    m_setupComplete = settings.value(PROPERTY_SETUP_COMPLETE_KEY, PROPERTY_SETUP_COMPLETE_DEFAULT).toBool();
    settings.endGroup();
}

void Application::saveProperty(const QString& key, const QVariant& value)
{
    static QSettings settings;
    settings.beginGroup(PROPERTIES_GROUP_NAME);
    settings.setValue(key, value);
    settings.endGroup();
    settings.sync();
}

void Application::registerDevice()
{
    if (!m_remoteApi.enabled()) {
        return;
    }

    DeviceStatus status;
    status.deviceId = m_remoteApi.deviceId();
    m_remoteApi.createObject(status, [](bool success, const QString& error) {
        if (success) {
            qDebug() << "Device registered successfully";
        }
        else {
            qWarning() << "Failed to register device:" << error;
        }
    });
}

bool Application::advanceToNextApp()
{
    if (m_currentAppIndex + 1 < m_applications.size()) {
        m_currentAppIndex++;
        return true;
    }
    return false;
}

Application::PanelType Application::getNextPanel(PanelType current) const
{
    switch (current) {
    case Welcome:
        return DeviceId;

    case DeviceId:
        return ServerConnection;

    case ServerConnection:
        if (!m_applications.isEmpty()) {
            return AppEnable;
        }
        return Finish;

    case AppEnable: {
        auto* app = currentApp();
        if (app && app->configuration() && app->configuration()->enabled()) {
            // If timer type, show date/time picker first
            if (app->type() == Common::Type::TimeElapsed || app->type() == Common::Type::Countdown) {
                return AppDateTime;
            }
            return AppBackground;
        }
        // App disabled - advance to next app or finish
        if (m_currentAppIndex + 1 < m_applications.size()) {
            return AppEnable; // next() will call advanceToNextApp()
        }
        return Finish;
    }

    case AppDateTime:
        return AppBackground;

    case AppBackground:
        return AppOpacity;

    case AppOpacity:
        return AppBaseColor;

    case AppBaseColor:
        return AppAccentColor;

    case AppAccentColor:
        if (m_currentAppIndex + 1 < m_applications.size()) {
            return AppEnable; // next() will call advanceToNextApp()
        }
        return Finish;

    case Finish:
    default:
        return Finish;
    }
}

void Application::showDialWheel(int min, int max, int step, int value)
{
    m_dialWheel.min = min;
    m_dialWheel.max = max;
    m_dialWheel.step = step;
    m_dialWheel.value = value;
    m_dialWheel.visible = true;

    emit dialWheelChanged();
}

void Application::updateDialWheelValue(int value)
{
    if (m_dialWheel.value == value) {
        return;
    }

    m_dialWheel.value = value;
    emit dialWheelChanged();
}

void Application::selectMedia(const QString& mediaName)
{
    auto* app = currentApp();
    if (app && app->configuration()) {
        app->configuration()->setBackground(mediaName);
    }
}

void Application::selectColor(const QColor& color)
{
    auto* app = currentApp();
    if (!app || !app->configuration()) {
        return;
    }

    if (m_currentPanel == AppBaseColor) {
        app->configuration()->setBaseColor(color);
    }
    else if (m_currentPanel == AppAccentColor) {
        app->configuration()->setAccentColor(color);
    }
}

void Application::showDateTimeComponentPicker(int component, int year, int month, int day, int hour, int minute, int second)
{
    int min = 0, max = 100, step = 1, value = 0;

    switch (component) {
    case 0: // Day
        min = 1;
        max = QDate(year, month, 1).daysInMonth();
        step = 1;
        value = day;
        break;
    case 1: // Month
        min = 1;
        max = 12;
        step = 1;
        value = month;
        break;
    case 2: // Year
        min = 1970;
        max = 2100;
        step = 1;
        value = year;
        break;
    case 3: // Hours
        min = 0;
        max = 23;
        step = 1;
        value = hour;
        break;
    case 4: // Minutes
        min = 0;
        max = 59;
        step = 1;
        value = minute;
        break;
    case 5: // Seconds
        min = 0;
        max = 59;
        step = 1;
        value = second;
        break;
    }

    showDialWheel(min, max, step, value);
}

QColor Application::pendulumBobColor() const
{
    return m_pendulumBobColor;
}

void Application::setPendulumBobColor(const QColor& color)
{
    if (m_pendulumBobColor == color) {
        return;
    }

    m_pendulumBobColor = color;
    emit pendulumBobColorChanged();
}

QColor Application::pendulumRodColor() const
{
    return m_pendulumRodColor;
}

void Application::setPendulumRodColor(const QColor& color)
{
    if (m_pendulumRodColor == color) {
        return;
    }

    m_pendulumRodColor = color;
    emit pendulumRodColorChanged();
}

QColor Application::pendulumBackgroundColor() const
{
    return m_pendulumBackgroundColor;
}

void Application::setPendulumBackgroundColor(const QColor& color)
{
    if (m_pendulumBackgroundColor == color) {
        return;
    }

    m_pendulumBackgroundColor = color;
    emit pendulumBackgroundColorChanged();
}

QColor Application::baseColor() const
{
    return m_baseColor;
}

void Application::setBaseColor(const QColor& color)
{
    if (m_baseColor == color) {
        return;
    }

    m_baseColor = color;
    emit baseColorChanged();
}

QColor Application::accentColor() const
{
    return m_accentColor;
}

void Application::setAccentColor(const QColor& color)
{
    if (m_accentColor == color) {
        return;
    }

    m_accentColor = color;
    emit accentColorChanged();
}

void Application::applySystemConfiguration(const QJsonObject& systemConfig)
{
    if (systemConfig.contains(PROPERTY_PENDULUM_BOB_COLOR_KEY)) {
        setPendulumBobColor(QColor(systemConfig[PROPERTY_PENDULUM_BOB_COLOR_KEY].toString()));
    }
    if (systemConfig.contains(PROPERTY_PENDULUM_ROD_COLOR_KEY)) {
        setPendulumRodColor(QColor(systemConfig[PROPERTY_PENDULUM_ROD_COLOR_KEY].toString()));
    }
    if (systemConfig.contains(PROPERTY_PENDULUM_BACKGROUND_COLOR_KEY)) {
        setPendulumBackgroundColor(QColor(systemConfig[PROPERTY_PENDULUM_BACKGROUND_COLOR_KEY].toString()));
    }
    if (systemConfig.contains(PROPERTY_BASE_COLOR_KEY)) {
        setBaseColor(QColor(systemConfig[PROPERTY_BASE_COLOR_KEY].toString()));
    }
    if (systemConfig.contains(PROPERTY_ACCENT_COLOR_KEY)) {
        setAccentColor(QColor(systemConfig[PROPERTY_ACCENT_COLOR_KEY].toString()));
    }
}

QJsonObject Application::buildSystemConfiguration() const
{
    QJsonObject systemConfig;
    systemConfig[PROPERTY_PENDULUM_BOB_COLOR_KEY] = m_pendulumBobColor.name();
    systemConfig[PROPERTY_PENDULUM_ROD_COLOR_KEY] = m_pendulumRodColor.name();
    systemConfig[PROPERTY_PENDULUM_BACKGROUND_COLOR_KEY] = m_pendulumBackgroundColor.name();
    systemConfig[PROPERTY_BASE_COLOR_KEY] = m_baseColor.name();
    systemConfig[PROPERTY_ACCENT_COLOR_KEY] = m_accentColor.name();
    return systemConfig;
}
