#pragma once

#include <QAbstractItemModel>
#include <QObject>
#include <QQuickItem>
#include <QVariantList>
#include <QVariantMap>

class QmlUtils : public QObject
{
    Q_OBJECT

  public:
    explicit QmlUtils(QObject* parent = nullptr);

    // Core property inspection functions
    Q_INVOKABLE QVariantList properties(QObject* aObject) const;
    Q_INVOKABLE QVariantMap propertyInformation(QObject* aObject, const QString& aPropertyName) const;
    Q_INVOKABLE QString enumValueName(QObject* aObject, const QString& aEnumPropertyName, int aEnumValue) const;

    // Model inspection functions
    Q_INVOKABLE bool isQmlSupportedModel(QObject* aObject) const;
    Q_INVOKABLE QStringList roleNames(QAbstractItemModel* aModel) const;
    Q_INVOKABLE QVariant modelData(QAbstractItemModel* aModel, int aRowIndex, const QString& aRole) const;
    Q_INVOKABLE QVariant modelData(QAbstractItemModel* aModel, int aRowIndex) const;
    Q_INVOKABLE int modelCount(QAbstractItemModel* aModel) const;

    // Map inspection functions
    Q_INVOKABLE bool isMapProperty(const QVariantMap& aPropertyInfo) const;
    Q_INVOKABLE QVariantList mapEntries(QObject* aObject, const QString& aPropertyName) const;

  private:
    static QmlUtils* mInstance;
};
