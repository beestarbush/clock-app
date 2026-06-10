#ifndef APPS_CURRENTDATE_H
#define APPS_CURRENTDATE_H

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

namespace Common::Communication::WebSocket::Client
{
class Service;
}

namespace Applications::CurrentDate
{
class Application : public Common::Application
{
    Q_OBJECT
    Q_PROPERTY(Common::Configuration* configuration READ configuration CONSTANT)
    Q_PROPERTY(QString currentDate READ currentDate NOTIFY currentDateChanged)
    Q_PROPERTY(int year READ year NOTIFY yearChanged)
    Q_PROPERTY(int month READ month NOTIFY monthChanged)
    Q_PROPERTY(int day READ day NOTIFY dayChanged)
    Q_PROPERTY(int dayOfWeek READ dayOfWeek NOTIFY dayOfWeekChanged)

  public:
    Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media, Services::DateTime::Service& dateTime, Common::Communication::WebSocket::Client::Service& webSocket, QObject* parent = nullptr);

    Common::Configuration* configuration() const override;
    void applyConfiguration(const QJsonObject& configuration) override;

    QString currentDate() const;
    int year() const;
    int month() const;
    int day() const;
    int dayOfWeek() const;

  signals:
    void currentDateChanged();
    void yearChanged();
    void monthChanged();
    void dayChanged();
    void dayOfWeekChanged();

  private slots:
    void notifyDateChanged();

  private:
    Common::Configuration* m_configuration;
    Services::Media::Service& m_media;
    Services::DateTime::Service& m_dateTime;
    QTimer* m_updateTimer;
};
} // namespace Applications::CurrentDate

#endif // APPS_CURRENTDATE_H
