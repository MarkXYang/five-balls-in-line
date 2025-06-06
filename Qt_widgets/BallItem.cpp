#include "BallItem.h"
#include <QPainter>

BallItem::BallItem(Ball* ball, const QPixmap& pixmap, QGraphicsItem* parent)
    : QGraphicsObject(parent), m_ball(ball), m_pixmap(pixmap) {
    // Store the associated Ball object and the pixmap
    // Position will be set via setPos() by MainWindow
}

Ball* BallItem::getBall() const {
    return m_ball;
}

QRectF BallItem::boundingRect() const {
    // Return the bounding rectangle of the item.
    // This should encompass the area where the item paints.
    // If m_pixmap is scaled, this should reflect the scaled size.
    // For now, assume m_pixmap is already scaled to desired size (e.g. CELL_SIZE x CELL_SIZE)
    if (m_pixmap.isNull()) {
        return QRectF(0, 0, DEFAULT_CELL_SIZE, DEFAULT_CELL_SIZE); // Fallback for null pixmap
    }
    return QRectF(m_pixmap.rect()); // Pixmap's own rectangle
}

void BallItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option); // Not using style options for now
    Q_UNUSED(widget); // Not using widget context for now

    if (m_pixmap.isNull()) {
        // Optional: Draw a placeholder if pixmap is missing
        painter->setBrush(Qt::magenta);
        painter->drawRect(boundingRect());
    } else {
        painter->drawPixmap(0, 0, m_pixmap);
    }
}
