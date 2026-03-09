#include <QObject>

#include "network/Driver.h"
#include "storage/Driver.h"

namespace Services
{
class Container;
}

namespace Drivers
{
class Container : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Network::Driver* network MEMBER m_network CONSTANT)
    Q_PROPERTY(Storage::Driver* storage MEMBER m_storage CONSTANT)

  public:
    friend class Services::Container;

    Container(QObject* parent = nullptr);

  private:
    Storage::Driver* m_storage;
    Network::Driver* m_network;
};
} // namespace Drivers