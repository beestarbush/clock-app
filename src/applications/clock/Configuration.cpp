#include "Configuration.h"
#include <QDebug>
#include <QJsonObject>
using namespace Applications::Clock;

const QString PROPERTY_HOUR_COLOR_KEY = QStringLiteral("hour-color");
const QColor PROPERTY_HOUR_COLOR_DEFAULT = QColor("#995000");
const QString PROPERTY_MINUTE_COLOR_KEY = QStringLiteral("minute-color");
const QColor PROPERTY_MINUTE_COLOR_DEFAULT = QColor("#005099");
const QString PROPERTY_SECOND_COLOR_KEY = QStringLiteral("second-color");
const QColor PROPERTY_SECOND_COLOR_DEFAULT = QColor("#009950");

Configuration::Configuration(QString name, QObject* parent)
    : Common::Configuration(name, parent),
      m_hourColor(PROPERTY_HOUR_COLOR_DEFAULT),
      m_minuteColor(PROPERTY_MINUTE_COLOR_DEFAULT),
      m_secondColor(PROPERTY_SECOND_COLOR_DEFAULT)
{
}

QJsonObject Configuration::toJson() const
{
    auto json = Common::Configuration::toJson();
    json[PROPERTY_HOUR_COLOR_KEY] = m_hourColor.name();
    json[PROPERTY_MINUTE_COLOR_KEY] = m_minuteColor.name();
    json[PROPERTY_SECOND_COLOR_KEY] = m_secondColor.name();

    return json;
}

void Configuration::fromJson(const QJsonObject& json)
{
    Common::Configuration::fromJson(json);

    if (json.contains(PROPERTY_HOUR_COLOR_KEY)) {
        setHourColor(QColor(json[PROPERTY_HOUR_COLOR_KEY].toString()));
    }
    if (json.contains(PROPERTY_MINUTE_COLOR_KEY)) {
        setMinuteColor(QColor(json[PROPERTY_MINUTE_COLOR_KEY].toString()));
    }
    if (json.contains(PROPERTY_SECOND_COLOR_KEY)) {
        setSecondColor(QColor(json[PROPERTY_SECOND_COLOR_KEY].toString()));
    }
}

QColor Configuration::hourColor() const
{
    return m_hourColor;
}

void Configuration::setHourColor(const QColor& hourColor)
{
    if (m_hourColor == hourColor) {
        return;
    }

    m_hourColor = hourColor;
    emit hourColorChanged();
}

QColor Configuration::minuteColor() const
{
    return m_minuteColor;
}

void Configuration::setMinuteColor(const QColor& minuteColor)
{
    if (m_minuteColor == minuteColor) {
        return;
    }

    m_minuteColor = minuteColor;
    emit minuteColorChanged();
}

QColor Configuration::secondColor() const
{
    return m_secondColor;
}

void Configuration::setSecondColor(const QColor& secondColor)
{
    if (m_secondColor == secondColor) {
        return;
    }

    m_secondColor = secondColor;
    emit secondColorChanged();
}

Configuration& Configuration::operator=(const Configuration& other)
{
    if (this != &other) {
        // Copy member variables
        Common::Configuration::operator=(other);

        setHourColor(other.m_hourColor);
        setMinuteColor(other.m_minuteColor);
        setSecondColor(other.m_secondColor);
    }
    return *this;
}

namespace Applications::Clock
{
QDebug operator<<(QDebug debug, const Configuration& config)
{
    QDebugStateSaver saver(debug);
    // Print base class properties first
    debug.nospace() << static_cast<const Common::Configuration&>(config);
    debug.nospace() << "\n";
    debug.nospace() << "Clock: (\n"
                    << " - hourColor=" << config.hourColor() << "\n"
                    << " - minuteColor=" << config.minuteColor() << "\n"
                    << " - secondColor=" << config.secondColor() << ")";
    return debug;
}
} // namespace Applications::Clock
