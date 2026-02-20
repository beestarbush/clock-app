#ifndef SERVICES_MEDIA_ITEM_H
#define SERVICES_MEDIA_ITEM_H

#include <QObject>
#include <QString>

namespace Services::Media
{
class Item : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString filename READ filename CONSTANT)
    Q_PROPERTY(QString path READ path CONSTANT)
    Q_PROPERTY(QString type READ type CONSTANT)
    Q_PROPERTY(int size READ size CONSTANT)
    Q_PROPERTY(bool isValid READ isValid CONSTANT)

  public:
    Item(QObject* parent = nullptr);
    Item(const QString& id, const QString& filename, const QString& path, const QString& type, int size, QObject* parent = nullptr);

    QString id() const { return m_id; }
    QString filename() const { return m_filename; }
    QString path() const { return m_path; }
    QString type() const { return m_type; }
    int size() const { return m_size; }
    bool isValid() const { return m_isValid; }

    void setId(const QString& id) { m_id = id; }
    void setFilename(const QString& filename) { m_filename = filename; }
    void setPath(const QString& path) { m_path = path; }
    void setType(const QString& type) { m_type = type; }
    void setSize(int size) { m_size = size; }
    void setIsValid(bool valid) { m_isValid = valid; }

  private:
    QString m_id;
    QString m_filename;
    QString m_path;
    QString m_type;
    int m_size;
    bool m_isValid;
};
} // namespace Services::Media

#endif // SERVICES_MEDIA_ITEM_H
