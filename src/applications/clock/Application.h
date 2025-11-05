#ifndef APPS_CLOCK_H
#define APPS_CLOCK_H

#include "Configuration.h"
#include "applications/common/Application.h"
#include <QObject>

namespace Services::Media
{
class Service;
}

namespace Applications::Clock
{
class Configuration;

class Application : public Common::Application
{
    Q_OBJECT
    Q_PROPERTY(Configuration* configuration READ configuration CONSTANT)

  public:
    Application(const QString& id, Common::Type type, const QString& displayName, int order, Common::Watchface watchface, Services::Media::Service& media, QObject* parent = nullptr);

    Configuration* configuration() const override;

    void applyConfiguration(const Configuration& configuration);
    void applyConfiguration(const QJsonObject& configuration);

  private:
    Configuration* m_configuration;
    Services::Media::Service& m_media;
};
} // namespace Applications::Clock

#endif // APPS_CLOCK_H