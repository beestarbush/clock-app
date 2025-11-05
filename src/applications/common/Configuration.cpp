#include "Configuration.h"
#include <QDebug>
#include <QJsonObject>
using namespace Common;

// Configuration property keys
const QString PROPERTY_ENABLED_KEY = QStringLiteral("enabled");
const bool PROPERTY_ENABLED_DEFAULT = true;
const QString PROPERTY_BACKGROUND_KEY = QStringLiteral("background");
const QString PROPERTY_BACKGROUND_DEFAULT = QStringLiteral("default.gif");
const QString PROPERTY_BACKGROUND_OPACITY_KEY = QStringLiteral("background-opacity");
const qreal PROPERTY_BACKGROUND_OPACITY_DEFAULT = 0.5;
const QString PROPERTY_BASE_COLOR_KEY = QStringLiteral("base-color");
const QColor PROPERTY_BASE_COLOR_DEFAULT = QColor("#02996c");
const QString PROPERTY_ACCENT_COLOR_KEY = QStringLiteral("accent-color");
const QColor PROPERTY_ACCENT_COLOR_DEFAULT = QColor("#BBBBBB");

Configuration::Configuration(const QString& name, QObject* parent)
    : QObject(parent),
      m_name(name),
      m_enabled(PROPERTY_ENABLED_DEFAULT),
      m_background(PROPERTY_BACKGROUND_DEFAULT),
      m_backgroundOpacity(PROPERTY_BACKGROUND_OPACITY_DEFAULT),
      m_baseColor(PROPERTY_BASE_COLOR_DEFAULT),
      m_accentColor(PROPERTY_ACCENT_COLOR_DEFAULT)
{
}

QJsonObject Configuration::toJson() const
{
    QJsonObject json;

    // Configuration
    json[PROPERTY_ENABLED_KEY] = m_enabled;
    json[PROPERTY_BACKGROUND_KEY] = m_background;
    json[PROPERTY_BACKGROUND_OPACITY_KEY] = m_backgroundOpacity;
    json[PROPERTY_BASE_COLOR_KEY] = m_baseColor.name();
    json[PROPERTY_ACCENT_COLOR_KEY] = m_accentColor.name();

    return json;
}

void Configuration::fromJson(const QJsonObject& json)
{
    // Configuration
    if (json.contains(PROPERTY_ENABLED_KEY)) {
        setEnabled(json[PROPERTY_ENABLED_KEY].toBool());
    }
    if (json.contains(PROPERTY_BACKGROUND_KEY)) {
        setBackground(json[PROPERTY_BACKGROUND_KEY].toString());
    }
    if (json.contains(PROPERTY_BACKGROUND_OPACITY_KEY)) {
        setBackgroundOpacity(json[PROPERTY_BACKGROUND_OPACITY_KEY].toDouble());
    }
    if (json.contains(PROPERTY_BASE_COLOR_KEY)) {
        setBaseColor(QColor(json[PROPERTY_BASE_COLOR_KEY].toString()));
    }
    if (json.contains(PROPERTY_ACCENT_COLOR_KEY)) {
        setAccentColor(QColor(json[PROPERTY_ACCENT_COLOR_KEY].toString()));
    }
}

// Configuration accessors

bool Configuration::enabled() const
{
    return m_enabled;
}

void Configuration::setEnabled(const bool& enabled)
{
    if (m_enabled == enabled) {
        return;
    }

    m_enabled = enabled;
    emit enabledChanged();
}

qreal Configuration::backgroundOpacity() const
{
    return m_backgroundOpacity;
}

void Configuration::setBackgroundOpacity(const qreal& backgroundOpacity)
{
    if (m_backgroundOpacity == backgroundOpacity) {
        return;
    }

    m_backgroundOpacity = backgroundOpacity;
    emit backgroundOpacityChanged();
}

QString Configuration::background() const
{
    return m_background;
}

void Configuration::setBackground(const QString& background)
{
    if (m_background == background) {
        return;
    }

    m_background = background;
    emit backgroundChanged();
}

QColor Configuration::baseColor() const
{
    return m_baseColor;
}

void Configuration::setBaseColor(const QColor& baseColor)
{
    if (m_baseColor == baseColor) {
        return;
    }

    m_baseColor = baseColor;
    emit baseColorChanged();
}

QColor Configuration::accentColor() const
{
    return m_accentColor;
}

void Configuration::setAccentColor(const QColor& accentColor)
{
    if (m_accentColor == accentColor) {
        return;
    }

    m_accentColor = accentColor;
    emit accentColorChanged();
}

Configuration& Configuration::operator=(const Configuration& other)
{
    if (this != &other) {
        // Copy member variables
        setEnabled(other.m_enabled);
        setBackgroundOpacity(other.m_backgroundOpacity);
        setBackground(other.m_background);
        setBaseColor(other.m_baseColor);
        setAccentColor(other.m_accentColor);
    }
    return *this;
}

namespace Common
{
QDebug operator<<(QDebug debug, const Configuration& config)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "Common::Configuration(\n"
                    << " - enabled=" << config.enabled() << "\n"
                    << " - background=" << config.background() << "\n"
                    << " - backgroundOpacity=" << config.backgroundOpacity() << "\n"
                    << " - baseColor=" << config.baseColor().name() << "\n"
                    << " - accentColor=" << config.accentColor().name() << ")";
    return debug;
}
} // namespace Common
