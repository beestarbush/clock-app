#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <QMap>
#include <QString>

namespace Common
{
class Application;

Q_NAMESPACE

enum class Type
{
    Clock,
    TimeElapsed,
    Countdown,
    PhotoFrame,
    DateDisplay,
    Unknown
};
Q_ENUM_NS(Type)

// String to Type mapping
inline Type typeFromString(const QString& typeStr)
{
    if (typeStr == "clock") {
        return Type::Clock;
    }
    else if (typeStr == "time-elapsed") {
        return Type::TimeElapsed;
    }
    else if (typeStr == "countdown") {
        return Type::Countdown;
    }
    else if (typeStr == "photo-frame") {
        return Type::PhotoFrame;
    }
    else if (typeStr == "date-display") {
        return Type::DateDisplay;
    }
    else {
        return Type::Unknown;
    }
}

inline QString typeToString(Type type)
{
    switch (type) {
    case Type::Clock:
        return "clock";
    case Type::TimeElapsed:
        return "time-elapsed";
    case Type::Countdown:
        return "countdown";
    case Type::PhotoFrame:
        return "photo-frame";
    case Type::DateDisplay:
        return "date-display";
    default:
        return "unknown";
    }
}

enum class Watchface
{
    AnalogClock,
    SevenSegment,
    RoundProgressBar,
    CountdownTimer,
    PhotoFrame,
    DateDisplay,
    None
};
Q_ENUM_NS(Watchface)

// String to Watchface mapping
inline Watchface watchfaceFromString(const QString& watchfaceStr)
{
    if (watchfaceStr == "clock") {
        return Watchface::AnalogClock;
    }
    else if (watchfaceStr == "seven-segment") {
        return Watchface::SevenSegment;
    }
    else if (watchfaceStr == "round-progress-bar") {
        return Watchface::RoundProgressBar;
    }
    else if (watchfaceStr == "countdown") {
        return Watchface::CountdownTimer;
    }
    else if (watchfaceStr == "photo-frame") {
        return Watchface::PhotoFrame;
    }
    else if (watchfaceStr == "date-display") {
        return Watchface::DateDisplay;
    }
    else {
        return Watchface::None;
    }
}

inline QString watchfaceToString(Watchface watchface)
{
    switch (watchface) {
    case Watchface::AnalogClock:
        return "clock";
    case Watchface::SevenSegment:
        return "seven-segment";
    case Watchface::RoundProgressBar:
        return "round-progress-bar";
    case Watchface::CountdownTimer:
        return "countdown";
    case Watchface::PhotoFrame:
        return "photo-frame";
    case Watchface::DateDisplay:
        return "date-display";
    default:
        return "";
    }
}

using DynamicApplicationMap = QMap<QString, Application*>;
} // namespace Common

#endif // APPLICATIONS_COMMON_TYPES_H