#ifndef SERVICES_WEBSOCKET_TYPES_H
#define SERVICES_WEBSOCKET_TYPES_H

#include <QString>

namespace Services::WebSocket
{
Q_NAMESPACE

enum class MessageType
{
    Request,
    Response,
    Publish,
    UnknownMessageType
};
Q_ENUM_NS(MessageType)

enum class Method
{
    Subscribe,
    Unsubscribe,
    GetConfig,
    GetMedia,
    GetTemperature,
    SetBrightness,
    SetVolume,
    SetDeviceId,
    Shutdown,
    Reboot,
    PlaySound,
    StopSound,
    UnknownMethod
};
Q_ENUM_NS(Method)

enum class Topic
{
    Configuration,
    Media,
    ApplicationStatus,
    Temperature,
    UnknownTopic
};
Q_ENUM_NS(Topic)

enum class PlayMode
{
    Concurrent,
    Queue,
    Replace
};
Q_ENUM_NS(PlayMode)

inline MessageType messageTypeFromString(const QString& typeStr)
{
    if (typeStr == "request") {
        return MessageType::Request;
    }
    else if (typeStr == "response") {
        return MessageType::Response;
    }
    else if (typeStr == "publish") {
        return MessageType::Publish;
    }
    else {
        return MessageType::UnknownMessageType;
    }
}

inline QString messageTypeToString(MessageType type)
{
    switch (type) {
    case MessageType::Request:
        return "request";
    case MessageType::Response:
        return "response";
    case MessageType::Publish:
        return "publish";
    default:
        return "unknown";
    }
}

inline QString methodToString(Method type)
{
    switch (type) {
    case Method::Subscribe:
        return "subscribe";
    case Method::Unsubscribe:
        return "unsubscribe";
    case Method::GetConfig:
        return "getConfig";
    case Method::GetMedia:
        return "getMedia";
    case Method::GetTemperature:
        return "getTemperature";
    case Method::SetBrightness:
        return "setBrightness";
    case Method::SetVolume:
        return "setVolume";
    case Method::SetDeviceId:
        return "setDeviceId";
    case Method::Shutdown:
        return "shutdown";
    case Method::Reboot:
        return "reboot";
    case Method::PlaySound:
        return "playSound";
    case Method::StopSound:
        return "stopSound";
    default:
        return "unknown";
    }
}

inline Topic topicFromString(const QString& topicStr)
{
    if (topicStr == "configuration") {
        return Topic::Configuration;
    }
    else if (topicStr == "media") {
        return Topic::Media;
    }
    else if (topicStr == "application-status") {
        return Topic::ApplicationStatus;
    }
    else if (topicStr == "temperature") {
        return Topic::Temperature;
    }
    else {
        return Topic::UnknownTopic;
    }
}

inline QString topicToString(Topic topic)
{
    switch (topic) {
    case Topic::Configuration:
        return "configuration";
    case Topic::Media:
        return "media";
    case Topic::ApplicationStatus:
        return "application-status";
    case Topic::Temperature:
        return "temperature";
    default:
        return "unknown";
    }
}

inline QString playModeToString(PlayMode mode)
{
    switch (mode) {
    case PlayMode::Concurrent:
        return "concurrent";
    case PlayMode::Queue:
        return "queue";
    case PlayMode::Replace:
        return "replace";
    default:
        return "unknown";
    }
}

} // namespace Services::WebSocket

#endif // SERVICES_WEBSOCKET_TYPES_H