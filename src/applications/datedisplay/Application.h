#ifndef APPS_DATEDISPLAY_H
#define APPS_DATEDISPLAY_H

#include "applications/common/Application.h"
#include "applications/common/Configuration.h"
#include <QObject>
#include <QTimer>

namespace Services::Media
{
class Service;
}

namespace Services::DateTime
{
class Service;
}

namespace Applications::DateDisplay
{
class Application : public Common::Application
{
    Q_OBJECT
    Q_PROPERTY(Common::Configuration* configuration READ configuration CONSTANT)
    Q_PROPERTY(QString currentDate READ currentDate NOTIFY currentDateChanged)

  public:
    Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media, Services::DateTime::Service& dateTime, QObject* parent = nullptr);

    Common::Configuration* configuration() const override;
    void applyConfiguration(const QJsonObject& configuration) override;

    QString currentDate() const;

  signals:
    void currentDateChanged();

  private:
    Common::Configuration* m_configuration;
    Services::Media::Service& m_media;
    Services::DateTime::Service& m_dateTime;
    QTimer* m_updateTimer;
};
} // namespace Applications::DateDisplay

#endif // APPS_DATEDISPLAY_H
