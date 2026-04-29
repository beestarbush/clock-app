#include "Service.h"
#include <QDateTime>

namespace Services::Logging
{
const QString DEFAULT_CATEGORY = QStringLiteral("default");
Service* Service::s_instance = nullptr;

Service::Service(QObject* parent)
    : QObject(parent),
      m_model(new Model(this)),
      m_previousHandler(nullptr)
{
    s_instance = this;
    m_previousHandler = qInstallMessageHandler(messageHandler);
}

Service::~Service()
{
    qInstallMessageHandler(m_previousHandler);
    s_instance = nullptr;
}

Model* Service::model() const
{
    return m_model;
}

void Service::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    // Capture all messages with their category
    if (s_instance && s_instance->m_model) {
        quint64 timestamp = QDateTime::currentMSecsSinceEpoch();

        // Use the actual Qt logging category from context
        QString category = DEFAULT_CATEGORY;
        if (context.category && qstrlen(context.category) > 0) {
            category = QString::fromLatin1(context.category);
        }

        // Convert QtMsgType to Priority
        int priority = Item::Info;
        switch (type) {
        case QtDebugMsg:
            priority = Item::Debug;
            break;
        case QtInfoMsg:
            priority = Item::Info;
            break;
        case QtWarningMsg:
            priority = Item::Warning;
            break;
        case QtCriticalMsg:
            priority = Item::Critical;
            break;
        case QtFatalMsg:
            priority = Item::Fatal;
            break;
        }

        s_instance->m_model->addLogEntry(timestamp, category, msg, priority);
    }

    // Call the previous handler if it exists (to maintain default logging behavior)
    if (s_instance && s_instance->m_previousHandler) {
        s_instance->m_previousHandler(type, context, msg);
    }
}
} // namespace Services::Logging
