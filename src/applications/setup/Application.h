#ifndef APPS_SETUP_APPLICATION_H
#define APPS_SETUP_APPLICATION_H

#include "applications/common/Types.h"
#include <QColor>
#include <QObject>
#include <QString>

namespace Services::RemoteApi
{
class Service;
}

namespace Services::Configuration
{
class Service;
}

namespace Common
{
class Application;
}

// Dial wheel parameters as a value type
struct DialWheelParams
{
    Q_GADGET
    Q_PROPERTY(bool visible MEMBER visible)
    Q_PROPERTY(int min MEMBER min)
    Q_PROPERTY(int max MEMBER max)
    Q_PROPERTY(int step MEMBER step)
    Q_PROPERTY(int value MEMBER value)

  public:
    bool visible = false;
    int min = 0;
    int max = 100;
    int step = 1;
    int value = 0;
};

// Media selection parameters as a value type
struct MediaSelectionParams
{
    Q_GADGET
    Q_PROPERTY(bool visible MEMBER visible)

  public:
    bool visible = false;
};

// Color selection parameters as a value type
struct ColorSelectionParams
{
    Q_GADGET
    Q_PROPERTY(bool visible MEMBER visible)
    Q_PROPERTY(QColor startColor MEMBER startColor)

  public:
    bool visible = false;
    QColor startColor = QColor("#ffffff");
};

namespace Applications::Setup
{
class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool setupComplete READ isSetupComplete NOTIFY setupCompleteChanged)
    Q_PROPERTY(PanelType currentPanel READ currentPanel NOTIFY currentPanelChanged)
    Q_PROPERTY(Common::Application* currentApp READ currentApp NOTIFY currentAppChanged)
    Q_PROPERTY(int currentAppIndex READ currentAppIndex NOTIFY currentAppChanged)
    Q_PROPERTY(int appCount READ appCount CONSTANT)
    Q_PROPERTY(DialWheelParams dialWheel READ dialWheel NOTIFY dialWheelChanged)
    Q_PROPERTY(MediaSelectionParams mediaSelection READ mediaSelection NOTIFY mediaSelectionChanged)
    Q_PROPERTY(ColorSelectionParams colorSelection READ colorSelection NOTIFY colorSelectionChanged)
    Q_PROPERTY(QColor pendulumBobColor READ pendulumBobColor WRITE setPendulumBobColor NOTIFY pendulumBobColorChanged)
    Q_PROPERTY(QColor pendulumRodColor READ pendulumRodColor WRITE setPendulumRodColor NOTIFY pendulumRodColorChanged)
    Q_PROPERTY(QColor pendulumBackgroundColor READ pendulumBackgroundColor WRITE setPendulumBackgroundColor NOTIFY pendulumBackgroundColorChanged)
    Q_PROPERTY(QColor baseColor READ baseColor WRITE setBaseColor NOTIFY baseColorChanged)
    Q_PROPERTY(QColor accentColor READ accentColor WRITE setAccentColor NOTIFY accentColorChanged)

  public:
    // Setup panel types - generic phases
    enum PanelType
    {
        Welcome = 0,
        DeviceId = 1,
        ServerConnection = 2,
        AppEnable = 3,
        AppDateTime = 4,
        AppBackground = 5,
        AppOpacity = 6,
        AppBaseColor = 7,
        AppAccentColor = 8,
        Finish = 9,
        PanelTypeCount = 10
    };
    Q_ENUM(PanelType)

    Application(Common::DynamicApplicationMap& applications, Services::RemoteApi::Service& remoteApi, Services::Configuration::Service& configurationService, QObject* parent = nullptr);

    bool isSetupComplete() const;

    PanelType currentPanel() const;
    Common::Application* currentApp() const;
    int currentAppIndex() const;
    int appCount() const;

    DialWheelParams dialWheel() const;
    MediaSelectionParams mediaSelection() const;
    ColorSelectionParams colorSelection() const;

    QColor pendulumBobColor() const;
    void setPendulumBobColor(const QColor& color);

    QColor pendulumRodColor() const;
    void setPendulumRodColor(const QColor& color);

    QColor pendulumBackgroundColor() const;
    void setPendulumBackgroundColor(const QColor& color);

    QColor baseColor() const;
    void setBaseColor(const QColor& color);

    QColor accentColor() const;
    void setAccentColor(const QColor& color);

    void applySystemConfiguration(const QJsonObject& systemConfig);
    QJsonObject buildSystemConfiguration() const;

    Q_INVOKABLE void finish();
    Q_INVOKABLE void next();
    Q_INVOKABLE void reset();

    // Dialog methods
    Q_INVOKABLE void showDialWheel(int min, int max, int step, int value);
    Q_INVOKABLE void updateDialWheelValue(int value);

    Q_INVOKABLE void selectMedia(const QString& mediaName);
    Q_INVOKABLE void selectColor(const QColor& color);

    // DateTime picker helper
    Q_INVOKABLE void showDateTimeComponentPicker(int component, int year, int month, int day, int hour, int minute, int second);

  signals:
    void setupCompleteChanged();
    void currentPanelChanged();
    void currentAppChanged();
    void dialWheelChanged();
    void mediaSelectionChanged();
    void colorSelectionChanged();
    void pendulumBobColorChanged();
    void pendulumRodColorChanged();
    void pendulumBackgroundColorChanged();
    void baseColorChanged();
    void accentColorChanged();

  private:
    void loadProperties();
    void saveProperty(const QString& key, const QVariant& value);
    void registerDevice();
    PanelType getNextPanel(PanelType current) const;
    bool advanceToNextApp();

    bool m_setupComplete;

    // Current panel state
    PanelType m_currentPanel;

    // Dialog state
    DialWheelParams m_dialWheel;
    MediaSelectionParams m_mediaSelection;
    ColorSelectionParams m_colorSelection;

    // Reference to the shared dynamic applications map
    Common::DynamicApplicationMap& m_applications;
    int m_currentAppIndex;

    // Services
    Services::RemoteApi::Service& m_remoteApi;
    Services::Configuration::Service& m_configurationService;

    // System configuration
    QColor m_pendulumBobColor;
    QColor m_pendulumRodColor;
    QColor m_pendulumBackgroundColor;
    QColor m_baseColor;
    QColor m_accentColor;
};
} // namespace Applications::Setup

#endif // APPS_SETUP_APPLICATION_H
