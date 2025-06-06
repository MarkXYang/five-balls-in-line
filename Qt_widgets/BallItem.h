#ifndef BALLITEM_H
#define BALLITEM_H

#include <QGraphicsObject> // Changed from QGraphicsPixmapItem
#include <QString>
#include <QPixmap>
#include <QPainter> // Needed for paint method
#include "Ball.h"

class BallItem : public QGraphicsObject { // Inherits QObject and QGraphicsItem
    Q_OBJECT
    // Q_PROPERTY(QPointF pos READ pos WRITE setPos) // "pos" is an existing property of QGraphicsItem

public:
    BallItem(Ball* ball, const QPixmap& pixmap, QGraphicsItem* parent = nullptr);

    Ball* getBall() const;

    // Required virtual functions for QGraphicsItem (already in QGraphicsObject's base)
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;


private:
    Ball* m_ball; // Pointer to the logical ball object, BallItem does not own it
    QPixmap m_pixmap; // Store the pixmap for painting

    static const int DEFAULT_CELL_SIZE = 50; // Fallback if not scaled, or use actual
};

#endif // BALLITEM_H
