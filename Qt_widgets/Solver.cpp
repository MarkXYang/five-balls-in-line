#include "Solver.h"
#include "Ball.h"   // Needed for ball->getColor()
#include "Grid.h"   // Needed for m_grid->getBallAt() and Grid::GRID_SIZE
#include "qtpoint_hash.h" // For qHash(QPoint)
#include <QSet>     // QSet is used for ballsInLinesSet
#include <QDebug>   // For optional debugging

Solver::Solver(Grid* grid) : m_grid(grid) {
    Q_ASSERT(m_grid != nullptr);
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

    // Define direction vectors for horizontal, vertical, and two diagonals
    QList<QPair<int, int>> directions;
    directions << QPair<int, int>(1, 0);  // Horizontal
    directions << QPair<int, int>(0, 1);  // Vertical
    directions << QPair<int, int>(1, 1);  // Diagonal (top-left to bottom-right)
    directions << QPair<int, int>(1, -1); // Diagonal (bottom-left to top-right)

    for (const auto& dir : directions) {
        QList<QPoint> linePoints = scanLine(lastMovedX, lastMovedY, dir.first, dir.second, color);
        if (linePoints.size() >= 5) {
            for (const QPoint& p : linePoints) {
                ballsInLinesSet.insert(p);
            }
        }
    }

    return QList<QPoint>(ballsInLinesSet.begin(), ballsInLinesSet.end());
}

QList<QPoint> Solver::scanLine(int startX, int startY, int dx, int dy, const QString& color) {
    QList<QPoint> currentLinePoints;

    // Add the starting ball's position
    currentLinePoints.append(QPoint(startX, startY));

    // Scan in the positive direction (dx, dy)
    int currentX = startX + dx;
    int currentY = startY + dy;
    while (currentX >= 0 && currentX < Grid::GRID_SIZE &&
           currentY >= 0 && currentY < Grid::GRID_SIZE) {
        Ball* ball = m_grid->getBallAt(currentX, currentY);
        if (ball && ball->getColor() == color) {
            currentLinePoints.append(QPoint(currentX, currentY));
            currentX += dx;
            currentY += dy;
        } else {
            break; // End of line in this direction
        }
    }

    // Scan in the negative direction (-dx, -dy)
    currentX = startX - dx;
    currentY = startY - dy;
    while (currentX >= 0 && currentX < Grid::GRID_SIZE &&
           currentY >= 0 && currentY < Grid::GRID_SIZE) {
        Ball* ball = m_grid->getBallAt(currentX, currentY);
        if (ball && ball->getColor() == color) {
            currentLinePoints.append(QPoint(currentX, currentY)); // Add to the list
            currentX -= dx;
            currentY -= dy;
        } else {
            break; // End of line in this direction
        }
    }

    // No need to check size here, checkForLines will do it.
    // This helper just returns all balls of the same color along the axis.
    // The actual check for >=5 is done in checkForLines after getting results from scanLine.
    // Correction: The problem description for checkLine (now scanLine) implies it should check size.
    // Let's stick to that: if line.size >= 5, return line, else empty.
    // This means checkForLines will directly add to ballsInLinesSet if scanLine returns non-empty.

    if (currentLinePoints.size() >= 5) {
        return currentLinePoints;
    }
    return QList<QPoint>(); // Return empty list if not 5 or more
}
