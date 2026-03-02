
#include "QrCodeImage.h"
#include <QPainter>

QrCodeImage::QrCodeImage(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{
    setFlag(QQuickItem::ItemHasContents, true);
    setAntialiasing(true);
}

QrCodeImage::~QrCodeImage()
{
    if (m_qrCode) {
        QRcode_free(m_qrCode);
    }
}

QString QrCodeImage::text() const
{
    return m_text;
}

void QrCodeImage::setText(const QString& text)
{
    if (text == m_text) {
        return;
    }

    m_text = text;
    emit textChanged();
    regenerate();
}

QColor QrCodeImage::foregroundColor() const
{
    return m_foregroundColor;
}

void QrCodeImage::setForegroundColor(const QColor& color)
{
    if (color == m_foregroundColor) {
        return;
    }

    m_foregroundColor = color;
    emit foregroundColorChanged();
    update();
}

QColor QrCodeImage::backgroundColor() const
{
    return m_backgroundColor;
}

void QrCodeImage::setBackgroundColor(const QColor& color)
{
    if (color == m_backgroundColor) {
        return;
    }

    m_backgroundColor = color;
    emit backgroundColorChanged();
    update();
}

void QrCodeImage::regenerate()
{
    if (m_qrCode) {
        QRcode_free(m_qrCode);
        m_qrCode = nullptr;
    }

    if (!m_text.isEmpty()) {
        m_qrCode = QRcode_encodeString(m_text.toUtf8().constData(), 0, QR_ECLEVEL_M, QR_MODE_8, 1);
    }

    update();
}

void QrCodeImage::paint(QPainter* painter)
{
    if (!m_qrCode) {
        return;
    }

    const int qrWidth = m_qrCode->width;
    const qreal cellSize = qMin(width(), height()) / static_cast<qreal>(qrWidth);
    const qreal xOffset = (width() - cellSize * qrWidth) / 2.0;
    const qreal yOffset = (height() - cellSize * qrWidth) / 2.0;

    painter->fillRect(QRectF(xOffset, yOffset, cellSize * qrWidth, cellSize * qrWidth), m_backgroundColor);

    painter->setPen(Qt::NoPen);
    painter->setBrush(m_foregroundColor);

    for (int y = 0; y < qrWidth; ++y) {
        for (int x = 0; x < qrWidth; ++x) {
            if (m_qrCode->data[y * qrWidth + x] & 1) {
                painter->drawRect(QRectF(xOffset + x * cellSize, yOffset + y * cellSize, cellSize, cellSize));
            }
        }
    }
}
