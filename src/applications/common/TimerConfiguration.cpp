#include "TimerConfiguration.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
using namespace Common;

const QString PROPERTY_INITIALIZED_KEY = QStringLiteral("initialized");
const bool PROPERTY_INITIALIZED_DEFAULT = false;
const QString PROPERTY_TIMESTAMP_KEY = QStringLiteral("timestamp");
constexpr quint64 PROPERTY_TIMESTAMP_DEFAULT = 0;
const QString PROPERTY_SOUND_FILE_KEY = QStringLiteral("sound-file");
const QString PROPERTY_SOUND_FILE_DEFAULT = QStringLiteral("");
const QString PROPERTY_MILESTONES_KEY = QStringLiteral("milestones");

TimerConfiguration::TimerConfiguration(QString name, QObject* parent)
    : Common::Configuration(name, parent),
      m_initialized(PROPERTY_INITIALIZED_DEFAULT),
      m_timestamp(PROPERTY_TIMESTAMP_DEFAULT),
      m_soundFile(PROPERTY_SOUND_FILE_DEFAULT),
      m_milestones({})
{
}

QJsonObject TimerConfiguration::toJson() const
{
    auto json = Common::Configuration::toJson();
    json[PROPERTY_INITIALIZED_KEY] = m_initialized;
    json[PROPERTY_TIMESTAMP_KEY] = static_cast<qint64>(m_timestamp);
    json[PROPERTY_SOUND_FILE_KEY] = m_soundFile;

    QJsonArray milestonesArray;
    for (const Common::Milestone& m : m_milestones) {
        QJsonObject obj;
        obj["value"] = static_cast<qint64>(m.value);
        obj["unit"] = m.unit;
        milestonesArray.append(obj);
    }
    json[PROPERTY_MILESTONES_KEY] = milestonesArray;

    return json;
}

void TimerConfiguration::fromJson(const QJsonObject& json)
{
    Common::Configuration::fromJson(json);

    if (json.contains(PROPERTY_INITIALIZED_KEY)) {
        setInitialized(json[PROPERTY_INITIALIZED_KEY].toBool());
    }
    if (json.contains(PROPERTY_TIMESTAMP_KEY)) {
        setTimestamp(static_cast<quint64>(json[PROPERTY_TIMESTAMP_KEY].toInteger()));
    }
    if (json.contains(PROPERTY_SOUND_FILE_KEY)) {
        setSoundFile(json[PROPERTY_SOUND_FILE_KEY].toString());
    }
    if (json.contains(PROPERTY_MILESTONES_KEY)) {
        QList<Common::Milestone> milestones;
        for (const auto& val : json[PROPERTY_MILESTONES_KEY].toArray()) {
            QJsonObject obj = val.toObject();
            Common::Milestone m;
            m.value = static_cast<quint64>(obj["value"].toInteger());
            m.unit = obj["unit"].toString("days");
            milestones.append(m);
        }
        setMilestones(milestones);
    }
}

bool TimerConfiguration::isInitialized() const
{
    return m_initialized;
}

void TimerConfiguration::setInitialized(const bool& initialized)
{
    if (m_initialized == initialized) {
        return;
    }

    m_initialized = initialized;
    emit initializedChanged();
}

quint64 TimerConfiguration::timestamp() const
{
    return m_timestamp;
}

void TimerConfiguration::setTimestamp(const quint64& timestamp)
{
    if (m_timestamp == timestamp) {
        return;
    }

    m_timestamp = timestamp;
    emit timestampChanged();
}

QString TimerConfiguration::soundFile() const
{
    return m_soundFile;
}

void TimerConfiguration::setSoundFile(const QString& soundFile)
{
    if (m_soundFile == soundFile) {
        return;
    }

    m_soundFile = soundFile;
    emit soundFileChanged();
}

QList<Common::Milestone> TimerConfiguration::milestones() const
{
    return m_milestones;
}

void TimerConfiguration::setMilestones(const QList<Common::Milestone>& milestones)
{
    if (m_milestones == milestones) {
        return;
    }

    m_milestones = milestones;
    emit milestonesChanged();
}

TimerConfiguration& TimerConfiguration::operator=(const TimerConfiguration& other)
{
    if (this != &other) {
        Common::Configuration::operator=(other);
        setInitialized(other.m_initialized);
        setTimestamp(other.m_timestamp);
        setSoundFile(other.m_soundFile);
        setMilestones(other.m_milestones);
    }
    return *this;
}

namespace Common
{
QDebug operator<<(QDebug debug, const TimerConfiguration& config)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << static_cast<const Common::Configuration&>(config);
    debug.nospace() << "\n";
    debug.nospace() << "Common::TimerConfiguration: (\n"
                    << " - initialized=" << config.isInitialized() << "\n"
                    << " - timestamp=" << config.timestamp() << "\n"
                    << " - soundFile=" << config.soundFile() << "\n"
                    << " - milestones=[";
    for (const Common::Milestone& m : config.milestones())
        debug.nospace() << "{" << m.value << " " << m.unit << "}";
    debug.nospace() << "])";
    return debug;
}
} // namespace Common
