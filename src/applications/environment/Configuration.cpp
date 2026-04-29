#include "Configuration.h"
#include <QDebug>
#include <QJsonObject>

using namespace Applications::Environment;

const QString PROPERTY_GOOD_THRESHOLD_KEY = QStringLiteral("good-threshold");
const int PROPERTY_GOOD_THRESHOLD_DEFAULT = 800;
const QString PROPERTY_WARN_THRESHOLD_KEY = QStringLiteral("warn-threshold");
const int PROPERTY_WARN_THRESHOLD_DEFAULT = 1200;
const QString PROPERTY_DANGER_THRESHOLD_KEY = QStringLiteral("danger-threshold");
const int PROPERTY_DANGER_THRESHOLD_DEFAULT = 1800;

Configuration::Configuration(const QString& name, QObject* parent)
    : Common::Configuration(name, parent),
      m_goodThreshold(PROPERTY_GOOD_THRESHOLD_DEFAULT),
      m_warnThreshold(PROPERTY_WARN_THRESHOLD_DEFAULT),
      m_dangerThreshold(PROPERTY_DANGER_THRESHOLD_DEFAULT)
{
}

QJsonObject Configuration::toJson() const
{
    auto json = Common::Configuration::toJson();
    json[PROPERTY_GOOD_THRESHOLD_KEY] = m_goodThreshold;
    json[PROPERTY_WARN_THRESHOLD_KEY] = m_warnThreshold;
    json[PROPERTY_DANGER_THRESHOLD_KEY] = m_dangerThreshold;

    return json;
}

void Configuration::fromJson(const QJsonObject& json)
{
    Common::Configuration::fromJson(json);

    if (json.contains(PROPERTY_GOOD_THRESHOLD_KEY)) {
        setGoodThreshold(json[PROPERTY_GOOD_THRESHOLD_KEY].toInt());
    }
    if (json.contains(PROPERTY_WARN_THRESHOLD_KEY)) {
        setWarnThreshold(json[PROPERTY_WARN_THRESHOLD_KEY].toInt());
    }
    if (json.contains(PROPERTY_DANGER_THRESHOLD_KEY)) {
        setDangerThreshold(json[PROPERTY_DANGER_THRESHOLD_KEY].toInt());
    }
}

int Configuration::goodThreshold() const
{
    return m_goodThreshold;
}

void Configuration::setGoodThreshold(int goodThreshold)
{
    if (m_goodThreshold == goodThreshold) {
        return;
    }

    m_goodThreshold = goodThreshold;
    emit goodThresholdChanged();
}

int Configuration::warnThreshold() const
{
    return m_warnThreshold;
}

void Configuration::setWarnThreshold(int warnThreshold)
{
    if (m_warnThreshold == warnThreshold) {
        return;
    }

    m_warnThreshold = warnThreshold;
    emit warnThresholdChanged();
}

int Configuration::dangerThreshold() const
{
    return m_dangerThreshold;
}

void Configuration::setDangerThreshold(int dangerThreshold)
{
    if (m_dangerThreshold == dangerThreshold) {
        return;
    }

    m_dangerThreshold = dangerThreshold;
    emit dangerThresholdChanged();
}

Configuration& Configuration::operator=(const Configuration& other)
{
    if (this != &other) {
        Common::Configuration::operator=(other);

        setGoodThreshold(other.m_goodThreshold);
        setWarnThreshold(other.m_warnThreshold);
        setDangerThreshold(other.m_dangerThreshold);
    }
    return *this;
}

namespace Applications::Environment
{
QDebug operator<<(QDebug debug, const Configuration& config)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << static_cast<const Common::Configuration&>(config);
    debug.nospace() << "\n";
    debug.nospace() << "Environment: (\n"
                    << " - goodThreshold=" << config.goodThreshold() << "\n"
                    << " - warnThreshold=" << config.warnThreshold() << "\n"
                    << " - dangerThreshold=" << config.dangerThreshold() << ")";
    return debug;
}
} // namespace Applications::Environment
