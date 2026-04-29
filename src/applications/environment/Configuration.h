#ifndef ENVIRONMENT_CONFIGURATION_H
#define ENVIRONMENT_CONFIGURATION_H

#include "applications/common/Configuration.h"
#include <QDebug>

namespace Applications::Environment
{
class Configuration : public Common::Configuration
{
    Q_OBJECT
    Q_PROPERTY(int goodThreshold READ goodThreshold WRITE setGoodThreshold NOTIFY goodThresholdChanged)
    Q_PROPERTY(int warnThreshold READ warnThreshold WRITE setWarnThreshold NOTIFY warnThresholdChanged)
    Q_PROPERTY(int dangerThreshold READ dangerThreshold WRITE setDangerThreshold NOTIFY dangerThresholdChanged)

  public:
    Configuration(const QString& name, QObject* parent = nullptr);

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;

    int goodThreshold() const;
    void setGoodThreshold(int goodThreshold);

    int warnThreshold() const;
    void setWarnThreshold(int warnThreshold);

    int dangerThreshold() const;
    void setDangerThreshold(int dangerThreshold);

    Configuration& operator=(const Configuration& other);
    friend QDebug operator<<(QDebug debug, const Configuration& config);

  signals:
    void goodThresholdChanged();
    void warnThresholdChanged();
    void dangerThresholdChanged();

  private:
    int m_goodThreshold;
    int m_warnThreshold;
    int m_dangerThreshold;
};
} // namespace Applications::Environment

#endif // ENVIRONMENT_CONFIGURATION_H
