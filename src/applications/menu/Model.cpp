#include "Model.h"
#include "Item.h"
using namespace Applications::Menu;

Model::Model(QObject* parent)
    : QAbstractListModel(parent)
{
}

int Model::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_items.count();
}

QVariant Model::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.count())
        return QVariant();

    auto* item = m_items[index.row()];
    switch (role) {
    case LabelRole:
        return item->label();
    case IconRole:
        return item->icon();
    case ItemRole:
        return QVariant::fromValue(item);
    }
    return QVariant();
}

QHash<int, QByteArray> Model::roleNames() const
{
    return {{LabelRole, "label"}, {IconRole, "icon"}, {ItemRole, "item"}};
}

Item* Model::get(int index) const
{
    return (index >= 0 && index < m_items.count()) ? m_items[index] : nullptr;
}

void Model::add(Item& item)
{
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count());
    m_items.append(&item);
    endInsertRows();
    emit countChanged();
}
