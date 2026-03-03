#ifndef APPS_TIME_ELAPSED_APPLICATION_H
#define APPS_TIME_ELAPSED_APPLICATION_H

#include "applications/common/TimerApplication.h"
#include <QObject>

namespace Applications::TimeElapsed
{
class Application : public Common::TimerApplication
{
    Q_OBJECT

  public:
    Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media, QObject* parent = nullptr);

  protected:
    void calculateTime() override;
};
} // namespace Applications::TimeElapsed

#endif // APPS_TIME_ELAPSED_APPLICATION_H
