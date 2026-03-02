#include "Container.h"
using namespace Drivers;

Container::Container(QObject* parent)
    : QObject(parent),
      m_storage(new Storage::Driver(this)),
      m_network(new Network::Driver(this)),
      m_screen(new Screen::Driver(this)),
      m_system(new System::Driver(this))
{
}
