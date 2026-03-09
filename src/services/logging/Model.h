#ifndef SERVICES_LOGGING_MODEL_H
#define SERVICES_LOGGING_MODEL_H

#include "Item.h"
#include <QAbstractListModel>
#include <QList>

namespace Services::Logging
{
class Model : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

  public:
    enum Roles
    {
        TimestampRole = Qt::UserRole + 1,
        CategoryRole,
        MessageRole,
        IdRole,
        PriorityRole
    };

    explicit Model(QObject* parent = nullptr);
    ~Model();

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Model manipulation
    Q_INVOKABLE Item* get(int index) const;
    Q_INVOKABLE void addLogEntry(quint64 timestamp, const QString& category, const QString& message, int priority);
    Q_INVOKABLE void clearAll();
    Q_INVOKABLE QStringList getCategories() const;

  signals:
    void countChanged();

  private:
    QList<Item*> m_logs;
};
} // namespace Services::Logging

#endif // SERVICES_LOGGING_MODEL_H
