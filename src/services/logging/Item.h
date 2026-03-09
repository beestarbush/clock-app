#ifndef SERVICES_LOGGING_ITEM_H
#define SERVICES_LOGGING_ITEM_H

#include <QObject>

namespace Services::Logging
{
class Item : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint64 id READ id CONSTANT)
    Q_PROPERTY(quint64 timestamp READ timestamp CONSTANT)
    Q_PROPERTY(QString category READ category CONSTANT)
    Q_PROPERTY(QString message READ message CONSTANT)
    Q_PROPERTY(QString priority READ priority CONSTANT)

  public:
    enum Priority
    {
        Debug,
        Info,
        Warning,
        Critical,
        Fatal
    };
    Q_ENUM(Priority)

    Item(QObject* parent = nullptr);
    Item(quint64 timestamp, const QString& category, const QString& message, Priority priority, QObject* parent = nullptr);

    quint64 id() const { return m_id; }
    quint64 timestamp() const { return m_timestamp; }
    QString category() const { return m_category; }
    QString message() const { return m_message; }
    QString priority() const { return priorityToString(m_priority); }
    Priority priorityLevel() const { return m_priority; }

    static QString priorityToString(Priority priority);

  private:
    static quint64 s_nextId;
    quint64 m_id;
    quint64 m_timestamp;
    QString m_category;
    QString m_message;
    Priority m_priority;
};
} // namespace Services::Logging

#endif // SERVICES_LOGGING_ITEM_H
