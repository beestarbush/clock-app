#include "Application.h"

#include <QLoggingCategory>
#include <algorithm>

Q_LOGGING_CATEGORY(WatchfaceApplication, "WatchfaceApplication")

namespace Applications::Watchface
{
Application::Application(const Common::DynamicApplicationMap& applications, QObject* parent)
    : QObject(parent),
      m_applications(applications),
      m_currentIndex(0),
      m_rotationTimer(new QTimer(this))
{
    updateEnabledWatchfaces();

    // Setup rotation timer (uses per-app duration)
    m_rotationTimer->setSingleShot(true);
    connect(m_rotationTimer, &QTimer::timeout, this, &Application::rotateToNext);

    if (!m_enabledWatchfaces.isEmpty()) {
        startTimerForCurrentApp();
        qCDebug(WatchfaceApplication) << "Watchface::Application initialized with" << m_enabledWatchfaces.size() << "enabled watchfaces";
    }
    else {
        qCDebug(WatchfaceApplication) << "Watchface::Application: No enabled watchfaces found";
    }
}

Common::Application* Application::currentApp() const
{
    if (m_enabledWatchfaces.isEmpty() || m_currentIndex >= m_enabledWatchfaces.size()) {
        return nullptr;
    }
    return m_enabledWatchfaces[m_currentIndex];
}

void Application::refresh()
{
    updateEnabledWatchfaces();

    if (!m_enabledWatchfaces.isEmpty()) {
        startTimerForCurrentApp();
        qCDebug(WatchfaceApplication) << "Watchface::Application refreshed with" << m_enabledWatchfaces.size() << "enabled watchfaces";
    }
    else {
        m_rotationTimer->stop();
        qCDebug(WatchfaceApplication) << "Watchface::Application refreshed: No enabled watchfaces found";
    }

    emit currentAppChanged();
}

void Application::nextWatchface()
{
    if (m_enabledWatchfaces.isEmpty()) {
        return;
    }

    m_currentIndex = (m_currentIndex + 1) % m_enabledWatchfaces.size();
    emit currentAppChanged();

    // Restart timer with new app's duration
    startTimerForCurrentApp();
}

void Application::previousWatchface()
{
    if (m_enabledWatchfaces.isEmpty()) {
        return;
    }

    m_currentIndex = (m_currentIndex - 1 + m_enabledWatchfaces.size()) % m_enabledWatchfaces.size();
    emit currentAppChanged();

    // Restart timer with new app's duration
    startTimerForCurrentApp();
}

void Application::updateEnabledWatchfaces()
{
    m_enabledWatchfaces.clear();

    for (auto it = m_applications.constBegin(); it != m_applications.constEnd(); ++it) {
        Common::Application* app = it.value();
        if (!app || app->type() == Common::Type::Unknown) {
            continue;
        }
        if (app->configuration() && !app->configuration()->enabled()) {
            continue;
        }
        m_enabledWatchfaces.append(app);
    }

    m_currentIndex = 0;

    // Sort by configured order
    std::sort(m_enabledWatchfaces.begin(), m_enabledWatchfaces.end(), [](const Common::Application* a, const Common::Application* b) {
        return a->order() < b->order();
    });
}

void Application::rotateToNext()
{
    nextWatchface();
}

void Application::startTimerForCurrentApp()
{
    auto* app = currentApp();
    int durationSeconds = 10; // fallback default
    if (app && app->configuration()) {
        durationSeconds = qBound(3, app->configuration()->duration(), 60);
    }
    m_rotationTimer->start(durationSeconds * 1000);
}

} // namespace Applications::Watchface

