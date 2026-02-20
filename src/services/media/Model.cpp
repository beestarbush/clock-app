#include "Model.h"
#include <QDir>
#include <QFileInfo>

using namespace Services::Media;

Model::Model(QObject* parent)
    : QAbstractListModel(parent)
{
}

int Model::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_items.size();
}

QVariant Model::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size())
        return QVariant();

    const Item* item = m_items.at(index.row());

    switch (role) {
    case IdRole:
        return item->id();
    case FilenameRole:
        return item->filename();
    case PathRole:
        return item->path();
    case TypeRole:
        return item->type();
    case SizeRole:
        return item->size();
    case IsValidRole:
        return item->isValid();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> Model::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[FilenameRole] = "filename";
    roles[PathRole] = "path";
    roles[TypeRole] = "type";
    roles[SizeRole] = "size";
    roles[IsValidRole] = "isValid";
    return roles;
}

Item* Model::get(int index) const
{
    return (index >= 0 && index < m_items.count()) ? m_items[index] : nullptr;
}

QString Model::getPath(const QString& filename) const
{
    for (const auto* item : m_items) {
        if (item->filename() == filename) {
            return item->path();
        }
    }
    return QString();
}

bool Model::contains(const QString& filename) const
{
    for (const auto* item : m_items) {
        if (item->filename() == filename) {
            return true;
        }
    }
    return false;
}

int Model::indexOf(const QString& filename) const
{
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i]->filename() == filename) {
            return i;
        }
    }
    return -1;
}

void Model::setMedia(const QList<Item*>& items)
{
    // Clear existing items
    if (!m_items.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_items);
        m_items.clear();
        endResetModel();
    }

    if (items.isEmpty()) {
        emit countChanged();
        return;
    }

    // Add new items
    beginInsertRows(QModelIndex(), 0, items.size() - 1);
    m_items = items;
    
    // Reparent items to this model to ensure cleanup
    for (Item* item : m_items) {
        if (item) {
            item->setParent(this);
        }
    }
    
    endInsertRows();

    emit countChanged();
}

void Model::addItem(Item* item)
{
    if (!item) return;

    int row = m_items.size();
    beginInsertRows(QModelIndex(), row, row);
    item->setParent(this);
    m_items.append(item);
    endInsertRows();

    emit countChanged();
}

void Model::removeItem(const QString& filename)
{
    int row = indexOf(filename);
    if (row < 0) return;

    beginRemoveRows(QModelIndex(), row, row);
    Item* item = m_items.takeAt(row);
    delete item;
    endRemoveRows();

    emit countChanged();
}

void Model::clear()
{
    if (m_items.isEmpty())
        return;

    beginResetModel();
    qDeleteAll(m_items);
    m_items.clear();
    endResetModel();

    emit countChanged();
}
