#ifndef CLOCKCONFIG_H
#define CLOCKCONFIG_H

#include "applications/common/Configuration.h"
#include <QColor>
#include <QDebug>

namespace Applications::Clock
{
class Configuration : public Common::Configuration
{
    Q_OBJECT
    Q_PROPERTY(QColor hourColor READ hourColor WRITE setHourColor NOTIFY hourColorChanged)
    Q_PROPERTY(QColor minuteColor READ minuteColor WRITE setMinuteColor NOTIFY minuteColorChanged)
    Q_PROPERTY(QColor secondColor READ secondColor WRITE setSecondColor NOTIFY secondColorChanged)

  public:
    Configuration(QString name, QObject* parent = nullptr);

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;

    QColor hourColor() const;
    void setHourColor(const QColor& hourColor);

    QColor minuteColor() const;
    void setMinuteColor(const QColor& minuteColor);

    QColor secondColor() const;
    void setSecondColor(const QColor& secondColor);

    Configuration& operator=(const Configuration& other);
    friend QDebug operator<<(QDebug debug, const Configuration& config);

  signals:
    void hourColorChanged();
    void minuteColorChanged();
    void secondColorChanged();

  private:
    QColor m_hourColor;
    QColor m_minuteColor;
    QColor m_secondColor;
};
} // namespace Applications::Clock

#endif // CLOCKCONFIG_H
