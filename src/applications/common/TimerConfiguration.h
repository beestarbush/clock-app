#ifndef COMMON_TIMER_CONFIG_H
#define COMMON_TIMER_CONFIG_H

#include "Configuration.h"
#include <QDebug>

namespace Common
{
class TimerConfiguration : public Common::Configuration
{
    Q_OBJECT
    Q_PROPERTY(bool initialized READ isInitialized WRITE setInitialized NOTIFY initializedChanged)
    Q_PROPERTY(quint64 timestamp READ timestamp WRITE setTimestamp NOTIFY timestampChanged)

  public:
    TimerConfiguration(QString name, QObject* parent = nullptr);

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;

    bool isInitialized() const;
    void setInitialized(const bool& initialized);

    quint64 timestamp() const;
    void setTimestamp(const quint64& timestamp);

    TimerConfiguration& operator=(const TimerConfiguration& other);
    friend QDebug operator<<(QDebug debug, const TimerConfiguration& config);

  signals:
    void initializedChanged();
    void timestampChanged();

  private:
    bool m_initialized;
    quint64 m_timestamp;
};
} // namespace Common

#endif // COMMON_TIMER_CONFIG_H
