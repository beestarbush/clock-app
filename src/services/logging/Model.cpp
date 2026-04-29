#include "Model.h"

namespace Services::Logging
{
namespace
{
constexpr int MAX_LOG_ENTRIES = 1000;
}

Model::Model(QObject* parent)
    : QAbstractListModel(parent)
{
}

Model::~Model()
{
    qDeleteAll(m_logs);
}

int Model::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_logs.count();
}

QVariant Model::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_logs.count())
        return QVariant();

    Item* item = m_logs.at(index.row());

    switch (role) {
    case TimestampRole:
        return item->timestamp();
    case CategoryRole:
        return item->category();
    case MessageRole:
        return item->message();
    case IdRole:
        return item->id();
    case PriorityRole:
        return item->priority();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> Model::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TimestampRole] = "timestamp";
    roles[CategoryRole] = "category";
    roles[MessageRole] = "message";
    roles[IdRole] = "id";
    roles[PriorityRole] = "priority";
    return roles;
}

Item* Model::get(int index) const
{
    if (index < 0 || index >= m_logs.count())
        return nullptr;
    return m_logs.at(index);
}

void Model::addLogEntry(quint64 timestamp, const QString& category, const QString& message, int priority)
{
    if (m_logs.count() >= MAX_LOG_ENTRIES) {
        beginRemoveRows(QModelIndex(), 0, 0);
        delete m_logs.takeFirst();
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), m_logs.count(), m_logs.count());
    m_logs.append(new Item(timestamp, category, message, static_cast<Item::Priority>(priority), this));
    endInsertRows();
    emit countChanged();
}

QStringList Model::getCategories() const
{
    QSet<QString> categories;
    for (const Item* item : m_logs) {
        if (!item->category().isEmpty()) {
            categories.insert(item->category());
        }
    }
    QStringList result = categories.values();
    result.sort();
    return result;
}

void Model::clearAll()
{
    if (m_logs.isEmpty())
        return;

    beginRemoveRows(QModelIndex(), 0, m_logs.count() - 1);
    qDeleteAll(m_logs);
    m_logs.clear();
    endRemoveRows();
    emit countChanged();
}
} // namespace Services::Logging
