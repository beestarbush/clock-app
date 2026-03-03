#ifndef APPS_COUNTDOWN_APPLICATION_H
#define APPS_COUNTDOWN_APPLICATION_H

#include "applications/common/TimerApplication.h"
#include <QObject>

namespace Applications::Countdown
{
class Application : public Common::TimerApplication
{
    Q_OBJECT

  public:
    Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media, QObject* parent = nullptr);

  signals:
    void countdownFinished();

  protected:
    void calculateTime() override;

  private:
    bool m_finished = false;
};
} // namespace Applications::Countdown

#endif // APPS_COUNTDOWN_APPLICATION_H
