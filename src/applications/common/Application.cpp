#include "Application.h"

using namespace Common;

Application::Application(const QString& id, Type type, const QString& displayName, int order, Watchface watchface, QObject* parent)
    : QObject(parent),
      m_id(id),
      m_type(type),
      m_displayName(displayName),
      m_order(order),
      m_watchface(watchface)
{
}

QString Application::id() const
{
    return m_id;
}

Type Application::type() const
{
    return m_type;
}

QString Application::displayName() const
{
    return m_displayName;
}

int Application::order() const
{
    return m_order;
}

Watchface Application::watchface() const
{
    return m_watchface;
}

namespace Common
{
QDebug operator<<(QDebug debug, const Application& app)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "Common application metadata(\n"
                    << " - id=" << app.id() << "\n"
                    << " - type=" << app.type() << "\n"
                    << " - displayName=" << app.displayName() << "\n"
                    << " - order=" << app.order() << "\n"
                    << " - watchface=" << app.watchface() << ")\n";
    return debug;
}
} // namespace Common
