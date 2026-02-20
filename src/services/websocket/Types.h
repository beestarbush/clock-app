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
    Unknown
};
Q_ENUM_NS(MessageType)

enum class Method
{
    Subscribe,
    Unsubscribe,
    GetConfig,
    GetMedia,
    Unknown
};
Q_ENUM_NS(Method)

enum class Topic
{
    Configuration,
    Media,
    ApplicationStatus,
    Unknown
};
Q_ENUM_NS(Topic)

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
        return MessageType::Unknown;
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
    else {
        return Topic::Unknown;
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
    default:
        return "unknown";
    }
}

} // namespace Services::WebSocket

#endif // SERVICES_WEBSOCKET_TYPES_H