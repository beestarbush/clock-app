#ifndef COMMON_TIMER_CONFIG_H
#define COMMON_TIMER_CONFIG_H

#include "Configuration.h"
#include <QDebug>
#include <QList>
#include <QString>

namespace Common
{

struct Milestone
{
    quint64 value;
    QString unit; // "days" or "years"

    bool operator==(const Milestone& other) const
    {
        return value == other.value && unit == other.unit;
    }
};

class TimerConfiguration : public Common::Configuration
{
    Q_OBJECT
    Q_PROPERTY(bool initialized READ isInitialized WRITE setInitialized NOTIFY initializedChanged)
    Q_PROPERTY(quint64 timestamp READ timestamp WRITE setTimestamp NOTIFY timestampChanged)
    Q_PROPERTY(QString soundFile READ soundFile WRITE setSoundFile NOTIFY soundFileChanged)
    Q_PROPERTY(QList<Common::Milestone> milestones READ milestones WRITE setMilestones NOTIFY milestonesChanged)

  public:
    TimerConfiguration(QString name, QObject* parent = nullptr);

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;

    bool isInitialized() const;
    void setInitialized(const bool& initialized);

    quint64 timestamp() const;
    void setTimestamp(const quint64& timestamp);

    QString soundFile() const;
    void setSoundFile(const QString& soundFile);

    QList<Milestone> milestones() const;
    void setMilestones(const QList<Milestone>& milestones);

    TimerConfiguration& operator=(const TimerConfiguration& other);
    friend QDebug operator<<(QDebug debug, const TimerConfiguration& config);

  signals:
    void initializedChanged();
    void timestampChanged();
    void soundFileChanged();
    void milestonesChanged();

  private:
    bool m_initialized;
    quint64 m_timestamp;
    QString m_soundFile;
    QList<Milestone> m_milestones;
};
} // namespace Common

#endif // COMMON_TIMER_CONFIG_H
