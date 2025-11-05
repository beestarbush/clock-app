#include "Item.h"
using namespace Applications::Menu;

Item::Item(QString label, QString icon, QObject* parent)
    : QObject(parent),
      m_label(std::move(label)),
      m_icon(std::move(icon))
{
}
