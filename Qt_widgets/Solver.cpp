// Qt_widgets/Solver.cpp
#include "Solver.h"
#include "Ball.h"
#include "Grid.h"
#include "qtpoint_hash.h" // For QSet<QPoint>
#include <QSet>
#include <QDebug> // For optional debugging

Solver::Solver(Grid* grid) : m_grid(grid) {
    Q_ASSERT(m_grid != nullptr);
}

// New: Scans in a single specified direction (dx, dy) from (startX, startY)
// for balls of the specified color. Includes the starting ball.
QList<QPoint> Solver::scanLineDirectional(int startX, int startY, int dx, int dy, const QString& color) {
    QList<QPoint> linePoints;

    // The starting ball (startX, startY) is the reference point.
    // Its color is `color`. It's always included.
    linePoints.append(QPoint(startX, startY));

    int currentX = startX + dx;
    int currentY = startY + dy;

    while (currentX >= 0 && currentX < Grid::GRID_SIZE &&
           currentY >= 0 && currentY < Grid::GRID_SIZE) {
        Ball* ball = m_grid->getBallAt(currentX, currentY);
        if (ball && ball->getColor() == color) {
            linePoints.append(QPoint(currentX, currentY));
            currentX += dx;
            currentY += dy;
        } else {
            break; // End of line in this direction
        }
    }
    return linePoints;
}

QList<QPoint> Solver::checkForLines(int lastMovedX, int lastMovedY) {
    if (!m_grid) {
        return QList<QPoint>();
    }

    Ball* movedBall = m_grid->getBallAt(lastMovedX, lastMovedY);
    if (!movedBall) {
        return QList<QPoint>(); // No ball at the specified location
    }

    QString color = movedBall->getColor();
    QSet<QPoint> ballsInLinesSet; // Use QSet<QPoint> to store unique points

    // Define primary axes directions. For each axis, we scan in both positive and negative directions.
    QList<QPair<int, int>> axes;
    axes << QPair<int, int>(1, 0);  // Horizontal axis
    axes << QPair<int, int>(0, 1);  // Vertical axis
    axes << QPair<int, int>(1, 1);  // Diagonal (top-left to bottom-right)
    axes << QPair<int, int>(1, -1); // Diagonal (bottom-left to top-right)

    for (const auto& axis : axes) {
        int dx = axis.first;
        int dy = axis.second;

        // Scan in the "positive" direction along the axis (dx, dy)
        // This list includes (lastMovedX, lastMovedY) as its first element.
        QList<QPoint> linePart1 = scanLineDirectional(lastMovedX, lastMovedY, dx, dy, color);

        // Scan in the "negative" direction along the axis (-dx, -dy)
        // This list also includes (lastMovedX, lastMovedY) as its first element.
        QList<QPoint> linePart2 = scanLineDirectional(lastMovedX, lastMovedY, -dx, -dy, color);

        // Combine the two parts.
        // (lastMovedX, lastMovedY) is present in both linePart1 and linePart2.
        // So, the total length of the line along this axis is linePart1.size() + linePart2.size() - 1.
        if ((linePart1.size() + linePart2.size() - 1) >= 5) {
            // Add all unique points from both parts to the set.
            // QSet handles uniqueness automatically.
            for (const QPoint& p : linePart1) {
                ballsInLinesSet.insert(p);
            }
            for (const QPoint& p : linePart2) {
                ballsInLinesSet.insert(p);
            }
        }
    }

    // Convert the set of points to a QList to return
    if (ballsInLinesSet.isEmpty()) {
        return QList<QPoint>();
    }
    return QList<QPoint>(ballsInLinesSet.begin(), ballsInLinesSet.end());
}
