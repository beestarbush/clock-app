#include "Application.h"
using namespace Applications::Menu;

Application::Application(QObject* parent)
    : QObject(parent),
      m_main(this),
      m_dialog(None),
      m_dialogParam(-1),
      m_mainItems{
          Item("Menu", "qrc:/Icons/home.svg", this),
          Item("Display brightness", "qrc:/Icons/brightness.svg", this),
          Item("Customize", "qrc:/Icons/customize.svg", this),
          Item("Notifications", "qrc:/Icons/notifications.svg", this),
          Item("Version", "qrc:/Icons/version.svg", this)}
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
    m_mainItems[MainNotifications].setAction([this]() {
        showDialog(Notifications);
    });
    m_mainItems[MainVersion].setAction([this]() {
        showDialog(Version);
    });
    m_mainItems[MainCustomize].setAction([this]() {
        showDialog(Customize);
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
    qDebug() << "Dial wheel value changed:" << value;
    // Handle dial wheel value change based on context
}
