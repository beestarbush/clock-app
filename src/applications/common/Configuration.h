#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#include <QColor>
#include <QDebug>
#include <QObject>

namespace Common
{
class Configuration : public QObject
{
    Q_OBJECT

    // Configuration properties
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(qreal backgroundOpacity READ backgroundOpacity WRITE setBackgroundOpacity NOTIFY backgroundOpacityChanged)
    Q_PROPERTY(QString background READ background WRITE setBackground NOTIFY backgroundChanged)
    Q_PROPERTY(QColor baseColor READ baseColor WRITE setBaseColor NOTIFY baseColorChanged)
    Q_PROPERTY(QColor accentColor READ accentColor WRITE setAccentColor NOTIFY accentColorChanged)

  public:
    Configuration(const QString& name, QObject* parent = nullptr);

    virtual QJsonObject toJson() const;
    virtual void fromJson(const QJsonObject& json);

    bool enabled() const;
    void setEnabled(const bool& enabled);

    qreal backgroundOpacity() const;
    void setBackgroundOpacity(const qreal& backgroundOpacity);

    QString background() const;
    void setBackground(const QString& background);

    QColor baseColor() const;
    void setBaseColor(const QColor& baseColor);

    QColor accentColor() const;
    void setAccentColor(const QColor& accentColor);

    Configuration& operator=(const Configuration& other);
    friend QDebug operator<<(QDebug debug, const Configuration& config);

  signals:
    void enabledChanged();
    void backgroundOpacityChanged();
    void backgroundChanged();
    void baseColorChanged();
    void accentColorChanged();

  protected:
    QString m_name;

  private:
    bool m_enabled;
    qreal m_backgroundOpacity;
    QString m_background;
    QColor m_baseColor;
    QColor m_accentColor;
};
} // namespace Common

#endif // COMMON_CONFIG_H
