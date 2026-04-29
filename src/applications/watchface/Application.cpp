#include "Application.h"
#include "applications/clock/Application.h"
#include "applications/countdown/Application.h"
#include "applications/currentdate/Application.h"
#include "applications/nooperation/Application.h"
#include "applications/timeelapsed/Application.h"

#include <QDebug>
#include <QLoggingCategory>
#include <algorithm>

Q_LOGGING_CATEGORY(WatchfaceApplication, "WatchfaceApplication")

namespace Applications
{
namespace Watchface
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
        if (!app)
            continue;
        if (app->configuration() && !app->configuration()->enabled())
            continue;

        switch (app->type()) {
        case Common::Type::Clock: {
            auto* castedApp = dynamic_cast<Applications::Clock::Application*>(app);
            if (!castedApp) {
                qCDebug(WatchfaceApplication) << "Watchface: Failed to cast application" << it.key() << "to Clock::Application";
                continue;
            }

            m_enabledWatchfaces.append(castedApp);
        } break;

        case Common::Type::TimeElapsed: {
            auto* castedApp = dynamic_cast<Applications::TimeElapsed::Application*>(app);
            if (!castedApp) {
                qCDebug(WatchfaceApplication) << "Watchface: Failed to cast application" << it.key() << "to TimeElapsed::Application";
                continue;
            }

            m_enabledWatchfaces.append(castedApp);
        } break;

        case Common::Type::Countdown: {
            auto* castedApp = dynamic_cast<Applications::Countdown::Application*>(app);
            if (!castedApp) {
                qCDebug(WatchfaceApplication) << "Watchface: Failed to cast application" << it.key() << "to Countdown::Application";
                continue;
            }

            m_enabledWatchfaces.append(castedApp);
        } break;

        case Common::Type::NoOperation: {
            auto* castedApp = dynamic_cast<Applications::NoOperation::Application*>(app);
            if (!castedApp) {
                qCDebug(WatchfaceApplication) << "Watchface: Failed to cast application" << it.key() << "to NoOperation::Application";
                continue;
            }

            m_enabledWatchfaces.append(castedApp);
        } break;

        case Common::Type::CurrentDate: {
            auto* castedApp = dynamic_cast<Applications::CurrentDate::Application*>(app);
            if (!castedApp) {
                qCDebug(WatchfaceApplication) << "Watchface: Failed to cast application" << it.key() << "to CurrentDate::Application";
                continue;
            }

            m_enabledWatchfaces.append(castedApp);
        } break;

        case Common::Type::Unknown:
        default:
            qCDebug(WatchfaceApplication) << "Watchface: Application" << it.key() << "is not of any type, skipping";
            continue;
        }
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

} // namespace Watchface
} // namespace Applications
