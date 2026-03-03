#ifndef QMLCOMPONENTS_QRCODEIMAGE_H
#define QMLCOMPONENTS_QRCODEIMAGE_H

#include <QColor>
#include <QQuickPaintedItem>
#include <qrencode.h>

class QrCodeImage : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor NOTIFY foregroundColorChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)

  public:
    QrCodeImage(QQuickItem* parent = nullptr);
    ~QrCodeImage();
    void paint(QPainter* painter) override;

    QString text() const;
    void setText(const QString& text);

    QColor foregroundColor() const;
    void setForegroundColor(const QColor& color);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor& color);

  signals:
    void textChanged();
    void foregroundColorChanged();
    void backgroundColorChanged();

  private:
    void regenerate();

    QString m_text;
    QRcode* m_qrCode = nullptr;
    QColor m_foregroundColor = Qt::black;
    QColor m_backgroundColor = Qt::white;
};

#endif // QMLCOMPONENTS_QRCODEIMAGE_H
