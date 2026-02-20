#include "Item.h"

using namespace Services::Media;

Item::Item(QObject* parent)
    : QObject(parent),
      m_size(0),
      m_isValid(false)
{
}

Item::Item(const QString& id, const QString& filename, const QString& path, const QString& type, int size, QObject* parent)
    : QObject(parent),
      m_id(id),
      m_filename(filename),
      m_path(path),
      m_type(type),
      m_size(size),
      m_isValid(true)
{
}
