#include <QMap>
#include <QObject>

#include "clock/Application.h"
#include "common/Application.h"
#include "common/Types.h"
#include "countdown/Application.h"
#include "debug/Application.h"
#include "menu/Application.h"
#include "setup/Application.h"
#include "timeelapsed/Application.h"
#include "watchface/Application.h"

namespace Services
{
class Container;
namespace Configuration
{
class Service;
}
namespace Media
{
class Service;
}
} // namespace Services

namespace Applications
{
class Container : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Setup::Application* setup MEMBER m_setup CONSTANT)
    Q_PROPERTY(Debug::Application* debug MEMBER m_debug CONSTANT)
    Q_PROPERTY(Menu::Application* menu MEMBER m_menu CONSTANT)
    Q_PROPERTY(Watchface::Application* watchface MEMBER m_watchface CONSTANT)
    Q_PROPERTY(bool reloading READ reloading NOTIFY reloadingChanged)
    Q_PROPERTY(Common::DynamicApplicationMap applications MEMBER m_applications CONSTANT)
    Q_PROPERTY(QList<QString> applicationIds READ applicationIds CONSTANT)

  public:
    Container(Services::Container& services, QObject* parent = nullptr);

    // Public accessors for dynamic applications
    Q_INVOKABLE Common::Application* application(const QString& id) const;
    QList<QString> applicationIds() const;
    bool reloading() const;

  signals:
    void reloadingChanged();

  private:
    void setReloading(bool reloading);

    // Factory method
    Common::Application* createApplication(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media);
    void reload(Services::Configuration::Service& configuration, Services::Media::Service& media);

  private:
    bool m_reloading = true;

    // Dynamic applications (must be declared before core applications that reference it)
    Common::DynamicApplicationMap m_applications;

    // Core applications
    Setup::Application* m_setup;
    Debug::Application* m_debug;
    Menu::Application* m_menu;
    Watchface::Application* m_watchface;
};
} // namespace Applications