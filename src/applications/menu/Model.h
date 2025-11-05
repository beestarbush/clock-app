#ifndef APPS_MENU_MODEL_H
#define APPS_MENU_MODEL_H

#include <QAbstractListModel>
#include <QList>

namespace Applications::Menu
{
class Item;

class Model : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

  public:
    enum Roles
    {
        LabelRole = Qt::UserRole + 1,
        IconRole,
        ItemRole
    };

    explicit Model(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE Item* get(int index) const;
    void add(Item& item);

  signals:
    void countChanged();

  private:
    QList<Item*> m_items;
};
} // namespace Applications::Menu

#endif // APPS_MENU_MODEL_H
