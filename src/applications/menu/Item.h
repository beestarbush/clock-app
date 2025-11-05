#ifndef APPS_MENU_ITEM_H
#define APPS_MENU_ITEM_H

#include <QObject>
#include <QString>
#include <functional>

namespace Applications::Menu
{
class Item : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString label READ label CONSTANT)
    Q_PROPERTY(QString icon READ icon CONSTANT)

  public:
    Item(QString label, QString icon = QString(), QObject* parent = nullptr);

    QString label() const
    {
        return m_label;
    }

    QString icon() const
    {
        return m_icon;
    }

    void setAction(std::function<void()> action)
    {
        m_action = std::move(action);
    }
    Q_INVOKABLE void trigger()
    {
        if (m_action)
            m_action();
    }

  private:
    QString m_label;
    QString m_icon;
    std::function<void()> m_action;
};
} // namespace Applications::Menu

#endif // APPS_MENU_ITEM_H
