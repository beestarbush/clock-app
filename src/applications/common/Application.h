#ifndef COMMON_APPLICATION_H
#define COMMON_APPLICATION_H

#include "Types.h"
#include <QDebug>
#include <QJsonObject>
#include <QObject>
#include <QString>

namespace Common
{
class Configuration;
/**
 * Base class for all applications.
 * Holds the metadata that identifies this application instance
 * and exposes it to QML.
 */
class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(Common::Type type READ type CONSTANT)
    Q_PROPERTY(QString displayName READ displayName CONSTANT)
    Q_PROPERTY(int order READ order CONSTANT)
    Q_PROPERTY(Common::Watchface watchface READ watchface CONSTANT)

  public:
    explicit Application(const QString& id, Type type, const QString& displayName, int order, Watchface watchface, QObject* parent = nullptr);
    virtual ~Application() = default;

    // Metadata accessors
    QString id() const;
    Type type() const;
    QString displayName() const;
    int order() const;
    Watchface watchface() const;

    // Apply configuration from JSON
    virtual void applyConfiguration(const QJsonObject& config) = 0;

    // Access to the common configuration
    virtual Common::Configuration* configuration() const = 0;

    friend QDebug operator<<(QDebug debug, const Application& app);

  private:
    QString m_id;
    Type m_type;
    QString m_displayName;
    int m_order;
    Watchface m_watchface;
};

} // namespace Common

#endif // COMMON_APPLICATION_H