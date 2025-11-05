#include "Application.h"
#include "applications/clock/Application.h"
#include "applications/countdown/Application.h"
#include "applications/timeelapsed/Application.h"

#include <QDebug>
#include <algorithm>

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

    // Setup rotation timer (10 seconds)
    m_rotationTimer->setInterval(10000);
    connect(m_rotationTimer, &QTimer::timeout, this, &Application::rotateToNext);

    if (!m_enabledWatchfaces.isEmpty()) {
        m_rotationTimer->start();
        qDebug() << "Watchface::Application initialized with" << m_enabledWatchfaces.size() << "enabled watchfaces";
    }
    else {
        qDebug() << "Watchface::Application: No enabled watchfaces found";
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
        m_rotationTimer->start();
        qDebug() << "Watchface::Application refreshed with" << m_enabledWatchfaces.size() << "enabled watchfaces";
    }
    else {
        m_rotationTimer->stop();
        qDebug() << "Watchface::Application refreshed: No enabled watchfaces found";
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

    // Restart timer
    m_rotationTimer->start();
}

void Application::previousWatchface()
{
    if (m_enabledWatchfaces.isEmpty()) {
        return;
    }

    m_currentIndex = (m_currentIndex - 1 + m_enabledWatchfaces.size()) % m_enabledWatchfaces.size();
    emit currentAppChanged();

    // Restart timer
    m_rotationTimer->start();
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
                qDebug() << "Watchface: Failed to cast application" << it.key() << "to Clock::Application";
                continue;
            }

            m_enabledWatchfaces.append(castedApp);
        } break;

        case Common::Type::TimeElapsed: {
            auto* castedApp = dynamic_cast<Applications::TimeElapsed::Application*>(app);
            if (!castedApp) {
                qDebug() << "Watchface: Failed to cast application" << it.key() << "to TimeElapsed::Application";
                continue;
            }

            m_enabledWatchfaces.append(castedApp);
        } break;

        case Common::Type::Countdown: {
            auto* castedApp = dynamic_cast<Applications::Countdown::Application*>(app);
            if (!castedApp) {
                qDebug() << "Watchface: Failed to cast application" << it.key() << "to Countdown::Application";
                continue;
            }

            m_enabledWatchfaces.append(castedApp);
        } break;

        case Common::Type::Unknown:
        default:
            qDebug() << "Watchface: Application" << it.key() << "is not of any type, skipping";
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

} // namespace Watchface
} // namespace Applications
