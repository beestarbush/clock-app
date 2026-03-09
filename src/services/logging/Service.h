#ifndef SERVICES_LOGGING_SERVICE_H
#define SERVICES_LOGGING_SERVICE_H

#include "Model.h"
#include <QObject>

namespace Services::Logging
{
class Service : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Model* model READ model CONSTANT)

  public:
    explicit Service(QObject* parent = nullptr);
    ~Service();

    Model* model() const;

  private:
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);
    static Service* s_instance;

    Model* m_model;
    QtMessageHandler m_previousHandler;
};
} // namespace Services::Logging

#endif // SERVICES_LOGGING_SERVICE_H
