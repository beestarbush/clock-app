#ifndef APPS_MENU_APPLICATION_H
#define APPS_MENU_APPLICATION_H

#include <QObject>
#include <QString>
#include <QVariant>

#include "Item.h"
#include "Model.h"

namespace Applications::Menu
{
class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Menu::Model* main READ main CONSTANT)
    Q_PROPERTY(Menu::Model* settings READ settings CONSTANT)
    Q_PROPERTY(DialogType dialog READ dialog NOTIFY dialogChanged)
    Q_PROPERTY(int dialogParam READ dialogParam NOTIFY dialogParamChanged)

  public:
    // Main menu items
    enum MainMenuItem
    {
        MainMenu = 0,
        MainSettings = 1,
        MainNotifications = 2,
        MainVersion = 3,
        MainMenuCount = 4
    };
    Q_ENUM(MainMenuItem)

    // Settings menu items
    enum SettingsMenuItem
    {
        SettingsBrightness = 0,
        SettingsWizard = 1,
        SettingsMenuCount = 2
    };
    Q_ENUM(SettingsMenuItem)

    // Dialog types
    enum DialogType
    {
        None,
        Version,
        SetupWizard,
        ScreenBrightness,
        Notifications,
        DialWheel
    };
    Q_ENUM(DialogType)

    Application(QObject* parent = nullptr);

    Menu::Model* main();
    Menu::Model* settings();

    DialogType dialog() const;
    int dialogParam() const;

    Q_INVOKABLE void showDialog(DialogType type, int param = -1);
    Q_INVOKABLE void closeDialog();
    Q_INVOKABLE void dialWheelValueChanged(int value);

  signals:
    void dialogChanged();
    void dialogParamChanged();

  private:
    void buildMenus();

    Menu::Model m_main;
    Menu::Model m_settings;

    DialogType m_dialog;
    int m_dialogParam;

    // Store menu items to manage lifetime - sized using enum counts
    Menu::Item m_mainItems[MainMenuCount];
    Menu::Item m_settingsItems[SettingsMenuCount];
};
} // namespace Applications::Menu

#endif // APPS_MENU_APPLICATION_H
