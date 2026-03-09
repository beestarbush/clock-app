#ifndef APPS_WATCHFACE_APPLICATION_H
#define APPS_WATCHFACE_APPLICATION_H

#include <QObject>
#include <QTimer>

#include "applications/common/Application.h"
#include "applications/common/Types.h"

namespace Applications::Watchface
{
class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Common::Application* currentApp READ currentApp NOTIFY currentAppChanged)

  public:
    Application(const Common::DynamicApplicationMap& applications, QObject* parent = nullptr);

    Common::Application* currentApp() const;

    void refresh();

    Q_INVOKABLE void nextWatchface();
    Q_INVOKABLE void previousWatchface();

  signals:
    void currentAppChanged();

  private:
    void updateEnabledWatchfaces();
    void rotateToNext();
    void startTimerForCurrentApp();

  private:
    const Common::DynamicApplicationMap& m_applications;
    QList<Common::Application*> m_enabledWatchfaces;
    int m_currentIndex;
    QTimer* m_rotationTimer;
};
} // namespace Applications::Watchface

#endif // APPS_WATCHFACE_APPLICATION_H

