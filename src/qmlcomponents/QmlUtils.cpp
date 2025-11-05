#include "QmlUtils.h"
#include <QDebug>
#include <QMetaEnum>
#include <QMetaObject>
#include <QMetaProperty>
#include <QQuickItem>

QmlUtils* QmlUtils::mInstance = nullptr;

QmlUtils::QmlUtils(QObject* parent)
    : QObject(parent)
{
    mInstance = this;
}

QVariantList QmlUtils::properties(QObject* aObject) const
{
    static auto lSuperClasses = QList<const QMetaObject*>()
                                << &QObject::staticMetaObject
                                << &QQuickItem::staticMetaObject;

    QVariantList lPropertiesList;

    if (aObject) {
        const auto lMetaObject = aObject->metaObject();
        const auto lPropertyCount = lMetaObject->propertyCount();

        auto lSuperClassMetaObject = lMetaObject;
        auto lPropertyOffset = 0;

        // Skip properties of QObject or QQuickItem class
        do {
            lPropertyOffset = lSuperClassMetaObject->propertyOffset();
            lSuperClassMetaObject = lSuperClassMetaObject->superClass();
        } while (lSuperClassMetaObject && !lSuperClasses.contains(lSuperClassMetaObject));

        for (int lI = lPropertyOffset; lI < lPropertyCount; ++lI) {
            lPropertiesList << propertyInformation(aObject, QString::fromLatin1(lMetaObject->property(lI).name()));
        }
    }

    // Sort properties by name
    std::sort(lPropertiesList.begin(), lPropertiesList.end(), [](const QVariant& aLeft, const QVariant& aRight) {
        return aLeft.toMap().value(QStringLiteral("name")).toString() < aRight.toMap().value(QStringLiteral("name")).toString();
    });

    return lPropertiesList;
}

QVariantMap QmlUtils::propertyInformation(QObject* aObject, const QString& aPropertyName) const
{
    QVariantMap lPropertyInformation;

    if (aObject) {
        const auto lMetaObject = aObject->metaObject();
        const auto lPropertyIndex = lMetaObject->indexOfProperty(qPrintable(aPropertyName));

        if (lPropertyIndex != -1) {
            const auto lMetaProperty = lMetaObject->property(lPropertyIndex);

            lPropertyInformation.insert(QStringLiteral("name"), aPropertyName);
            lPropertyInformation.insert(QStringLiteral("isEnumType"), lMetaProperty.isEnumType());
            lPropertyInformation.insert(QStringLiteral("isValid"), lMetaProperty.isValid());
            lPropertyInformation.insert(QStringLiteral("isReadable"), lMetaProperty.isReadable());
            lPropertyInformation.insert(QStringLiteral("isWritable"), lMetaProperty.isWritable());
            lPropertyInformation.insert(QStringLiteral("hasNotifySignal"), lMetaProperty.hasNotifySignal());

            if (strcmp(lMetaProperty.typeName(), "QVariant") == 0) {
                auto lVariant = lMetaProperty.read(aObject);
                lPropertyInformation.insert(QStringLiteral("variantTypeName"), QString::fromLatin1(lVariant.typeName()));
            }

            lPropertyInformation.insert(QStringLiteral("typeName"), QString::fromLatin1(lMetaProperty.typeName()));
        }
    }

    return lPropertyInformation;
}

QString QmlUtils::enumValueName(QObject* aObject, const QString& aEnumPropertyName, int aEnumValue) const
{
    if (aObject) {
        const auto lMetaObject = aObject->metaObject();
        auto lPropertyIndex = lMetaObject->indexOfProperty(qPrintable(aEnumPropertyName));

        if (lPropertyIndex == -1) {
            return {};
        }

        auto lMetaProperty = lMetaObject->property(lPropertyIndex);

        if (!lMetaProperty.isEnumType()) {
            return {};
        }

        const auto lMetaEnum = lMetaProperty.enumerator();
        return QString::fromLatin1(lMetaEnum.valueToKey(aEnumValue));
    }

    return {};
}

bool QmlUtils::isQmlSupportedModel(QObject* aObject) const
{
    auto lModel = dynamic_cast<QAbstractItemModel*>(aObject);
    return lModel && !lModel->roleNames().isEmpty();
}

QStringList QmlUtils::roleNames(QAbstractItemModel* aModel) const
{
    QStringList lResult;

    if (aModel) {
        const auto lRoles = aModel->roleNames().values();
        for (const auto& lRole : lRoles) {
            lResult << QString::fromLatin1(lRole);
        }
    }

    lResult.sort();

    return lResult;
}

QVariant QmlUtils::modelData(QAbstractItemModel* aModel, int aRowIndex, const QString& aRole) const
{
    if (aModel) {
        const auto lRoleIndex = aModel->roleNames().key(qPrintable(aRole));
        return aModel->data(aModel->index(aRowIndex, 0), lRoleIndex);
    }

    return {};
}

QVariant QmlUtils::modelData(QAbstractItemModel* aModel, int aRowIndex) const
{
    if (aModel) {
        QVariantMap lResult;
        const auto lRoleName = aModel->roleNames();

        for (auto lIterator = lRoleName.constBegin(); lIterator != lRoleName.constEnd(); ++lIterator) {
            auto lValue = aModel->data(aModel->index(aRowIndex, 0), lIterator.key());
            lResult.insert(QString::fromLocal8Bit(lIterator.value()), lValue);
        }

        return lResult;
    }

    return {};
}

int QmlUtils::modelCount(QAbstractItemModel* aModel) const
{
    return aModel ? aModel->rowCount() : 0;
}

bool QmlUtils::isMapProperty(const QVariantMap& aPropertyInfo) const
{
    auto lTypeName = aPropertyInfo.value(QStringLiteral("typeName")).toString();
    auto lVariantTypeName = aPropertyInfo.value(QStringLiteral("variantTypeName")).toString();
    return lTypeName.contains(QLatin1String("Map"), Qt::CaseInsensitive) || lVariantTypeName.contains(QLatin1String("Map"), Qt::CaseInsensitive);
}

QVariantList QmlUtils::mapEntries(QObject* aObject, const QString& aPropertyName) const
{
    QVariantList lEntries;

    if (!aObject) {
        return lEntries;
    }

    QVariant lValue = aObject->property(qPrintable(aPropertyName));

    if (!lValue.canConvert<QVariantMap>()) {
        return lEntries;
    }

    QVariantMap lMap = lValue.toMap();

    for (auto lIterator = lMap.constBegin(); lIterator != lMap.constEnd(); ++lIterator) {
        QVariantMap lEntry;
        lEntry.insert(QStringLiteral("key"), lIterator.key());

        auto lEntryValue = lIterator.value();
        auto* lObjPtr = lEntryValue.value<QObject*>();

        if (lObjPtr) {
            lEntry.insert(QStringLiteral("value"), QVariant::fromValue(lObjPtr));
            lEntry.insert(QStringLiteral("displayValue"),
                          QString::fromLatin1("%1 (0x%2)")
                              .arg(QString::fromLatin1(lObjPtr->metaObject()->className()),
                                   QString::number(reinterpret_cast<quintptr>(lObjPtr), 16)));
            lEntry.insert(QStringLiteral("isPointer"), true);
        }
        else {
            lEntry.insert(QStringLiteral("value"), lEntryValue);
            lEntry.insert(QStringLiteral("displayValue"), lEntryValue.toString());
            lEntry.insert(QStringLiteral("isPointer"), false);
        }

        lEntries.append(lEntry);
    }

    return lEntries;
}
