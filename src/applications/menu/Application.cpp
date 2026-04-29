#include "Application.h"
#include "services/websocket/Service.h"
#include "services/websocket/Types.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(MenuApplication, "MenuApplication")

using namespace Applications::Menu;

Application::Application(Services::WebSocket::Service& websocket, QObject* parent)
    : QObject(parent),
      m_main(this),
      m_dialog(None),
      m_dialogParam(-1),
      m_websocket(websocket),
      m_mainItems{
          Item("Menu", "qrc:/Icons/home.svg", this),
          Item("Display brightness", "qrc:/Icons/brightness.svg", this),
          Item("Volume", "qrc:/Icons/speaker.svg", this),
          Item("Customize", "qrc:/Icons/customize.svg", this),
          Item("Notifications", "qrc:/Icons/notifications.svg", this),
          Item("Version", "qrc:/Icons/version.svg", this),
          Item("Power off", "qrc:/Icons/power-off.svg", this)}
{
    buildMenus();
}

Model* Application::main()
{
    return &m_main;
}

Application::DialogType Application::dialog() const
{
    return m_dialog;
}

int Application::dialogParam() const
{
    return m_dialogParam;
}

void Application::buildMenus()
{
    // Build main menu using enum indices
    m_mainItems[MainMenu].setAction([this]() {
        closeDialog();
    });
    m_mainItems[MainBrightness].setAction([this]() {
        showDialog(ScreenBrightness);
    });
    m_mainItems[MainVolume].setAction([this]() {
        showDialog(Volume);
    });
    m_mainItems[MainNotifications].setAction([this]() {
        showDialog(Notifications);
    });
    m_mainItems[MainVersion].setAction([this]() {
        showDialog(Version);
    });
    m_mainItems[MainCustomize].setAction([this]() {
        showDialog(Customize);
    });
    m_mainItems[MainPowerOff].setAction([this]() {
        showDialog(PowerOff);
    });

    for (auto& item : m_mainItems)
        m_main.add(item);
}

void Application::showDialog(DialogType type, int param)
{
    m_dialog = type;
    m_dialogParam = param;
    emit dialogChanged();
    emit dialogParamChanged();
}

void Application::closeDialog()
{
    m_dialog = None;
    m_dialogParam = -1;
    emit dialogChanged();
    emit dialogParamChanged();
}

void Application::dialWheelValueChanged(int value)
{
    // Handle dial wheel value change based on context
}

void Application::shutdown()
{
    m_websocket.request(Services::WebSocket::Method::Shutdown, QJsonObject(), [](bool success, const QJsonObject&, const QString& error) {
        if (success) {
            qCInfo(MenuApplication) << "System shutdown initiated";
        }
        else {
            qCWarning(MenuApplication) << "Failed to shutdown system:" << error;
        }
    });
}
