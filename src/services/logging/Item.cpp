#include "Item.h"

namespace Services::Logging
{
quint64 Item::s_nextId = 1;

Item::Item(QObject* parent)
    : QObject(parent)
    , m_id(s_nextId++)
    , m_timestamp(0)
    , m_priority(Info)
{
}

Item::Item(quint64 timestamp, const QString& category, const QString& message, Priority priority, QObject* parent)
    : QObject(parent)
    , m_id(s_nextId++)
    , m_timestamp(timestamp)
    , m_category(category)
    , m_message(message)
    , m_priority(priority)
{
}

QString Item::priorityToString(Priority priority)
{
    switch (priority) {
    case Debug: return QStringLiteral("debug");
    case Info: return QStringLiteral("info");
    case Warning: return QStringLiteral("warning");
    case Critical: return QStringLiteral("critical");
    case Fatal: return QStringLiteral("fatal");
    default: return QStringLiteral("unknown");
    }
}
} // namespace Services::Logging
