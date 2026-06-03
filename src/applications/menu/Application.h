#ifndef APPS_MENU_APPLICATION_H
#define APPS_MENU_APPLICATION_H

#include <QObject>
#include <QString>
#include <QVariant>

#include "websocket/client/Service.h"

#include "Item.h"
#include "Model.h"

namespace Applications::Menu
{
class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Menu::Model* main READ main CONSTANT)
    Q_PROPERTY(DialogType dialog READ dialog NOTIFY dialogChanged)
    Q_PROPERTY(int dialogParam READ dialogParam NOTIFY dialogParamChanged)

  public:
    // Main menu items
    enum MainMenuItem
    {
        MainMenu = 0,
        MainBrightness = 1,
        MainVolume = 2,
        MainCustomize = 3,
        MainNotifications = 4,
        MainVersion = 5,
        MainPowerOff = 6,
        MainMenuCount = 7
    };
    Q_ENUM(MainMenuItem)

    // Dialog types
    enum DialogType
    {
        None,
        Version,
        ScreenBrightness,
        Volume,
        Customize,
        Notifications,
        DialWheel,
        PowerOff
    };
    Q_ENUM(DialogType)

    Application(Common::Communication::WebSocket::Client::Service& websocket, QObject* parent = nullptr);

    Menu::Model* main();

    DialogType dialog() const;
    int dialogParam() const;

    Q_INVOKABLE void showDialog(DialogType type, int param = -1);
    Q_INVOKABLE void closeDialog();
    Q_INVOKABLE void dialWheelValueChanged(int value);
    Q_INVOKABLE void shutdown();

  signals:
    void dialogChanged();
    void dialogParamChanged();

  private:
    void buildMenus();

    Menu::Model m_main;

    DialogType m_dialog;
    int m_dialogParam;

    Common::Communication::WebSocket::Client::Service& m_websocket;

    // Store menu items to manage lifetime - sized using enum counts
    Menu::Item m_mainItems[MainMenuCount];
};
} // namespace Applications::Menu

#endif // APPS_MENU_APPLICATION_H
