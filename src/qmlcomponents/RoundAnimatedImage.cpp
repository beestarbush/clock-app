
#include "RoundAnimatedImage.h"
#include <QDebug>
#include <QLoggingCategory>
#include <QPainterPath>

Q_LOGGING_CATEGORY(RoundAnimatedImageComponent, "RoundAnimatedImageComponent")

RoundAnimatedImage::RoundAnimatedImage(QQuickItem* parent)
    : QQuickPaintedItem(parent),
      m_movie(nullptr)
{
    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptedMouseButtons(Qt::NoButton);
    setOpacity(1.0);
    setAntialiasing(true);
}

QString RoundAnimatedImage::source() const
{
    return m_source;
}

void RoundAnimatedImage::setSource(const QString& path)
{
    // Store the original path
    QString oldSource = m_source;
    m_source = path;

    // Convert qrc:/ to :/ for Qt resource compatibility, but use local variable
    QString actualPath = path;
    if (actualPath.startsWith("qrc:/")) {
        actualPath.replace(0, 4, ":");
    }

    // Only skip reload if both the path and the actual loaded source are identical
    // This allows reloading when files change on disk
    if (oldSource == path && m_movie && m_movie->fileName() == actualPath) {
        return;
    }

    emit sourceChanged();

    if (m_movie) {
        m_movie->stop();
        delete m_movie;
        m_movie = nullptr;
    }

    // Handle empty path gracefully
    if (actualPath.isEmpty()) {
        update(); // Clear the display
        return;
    }

    m_movie = new QMovie(actualPath);
    if (!m_movie->isValid()) {
        qCWarning(RoundAnimatedImageComponent) << "Invalid movie source:" << actualPath;
        delete m_movie;
        m_movie = nullptr;
        update(); // Clear the display
        return;
    }
    connect(m_movie, &QMovie::frameChanged, this, &RoundAnimatedImage::onFrameChanged);
    if (isVisible()) {
        m_movie->start();
    }
}

void RoundAnimatedImage::onFrameChanged(int)
{
    update(); // triggers paint()
}

void RoundAnimatedImage::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& value)
{
    QQuickPaintedItem::itemChange(change, value);
    if (change == QQuickItem::ItemVisibleHasChanged && m_movie) {
        if (isVisible()) {
            if (m_movie->state() != QMovie::Running) {
                m_movie->start();
            }
        }
        else {
            m_movie->stop();
        }
    }
}

void RoundAnimatedImage::paint(QPainter* painter)
{
    QRectF bounds = boundingRect();

    // Enable antialiasing for smooth circle
    painter->setRenderHint(QPainter::Antialiasing, true);

    // Create circular clip
    QPainterPath path;
    path.addEllipse(bounds);
    painter->setClipPath(path);

    if (m_movie && m_movie->isValid()) {
        QImage frame = m_movie->currentImage();
        if (frame.isNull()) {
            // Draw black background for null frame
            painter->fillRect(bounds, Qt::black);
            return;
        }

        // Scale the frame to fill the bounding rect, keeping aspect ratio
        QImage scaledFrame = frame.scaled(bounds.size().toSize(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        // Center the image if aspect ratio does not match
        QPointF topLeft(
            bounds.x() + (bounds.width() - scaledFrame.width()) / 2,
            bounds.y() + (bounds.height() - scaledFrame.height()) / 2);

        painter->drawImage(bounds, frame);
    }
    else {
        // Draw black background when there's no valid movie source
        painter->fillRect(bounds, Qt::black);
    }
}
