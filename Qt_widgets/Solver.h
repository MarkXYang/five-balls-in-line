#ifndef SOLVER_H
#define SOLVER_H

#include <QList>
#include <QPoint>
#include <QSet> // For ensuring unique points if a ball is part of multiple lines

// Forward declaration
class Grid;
class Ball; // Ball might be needed if we were returning Ball*

class Solver {
public:
    Solver(Grid* grid); // Grid will be modified (balls removed)

    // Checks for lines of 5 or more same-colored balls involving the ball at (lastMovedX, lastMovedY).
    // Returns a list of QPoint positions of all balls that are part of such lines.
    QList<QPoint> checkForLines(int lastMovedX, int lastMovedY);

private:
    Grid* m_grid; // Non-const pointer to the grid

    // Helper to scan in one axis (e.g., horizontal, vertical, or a diagonal)
    // startX, startY: The position of the ball that just moved.
    // dx, dy: The direction vector for the line (e.g., (1,0) for horizontal).
    // color: The color of the ball to match.
    // Returns a list of QPoint positions of balls forming a line of >= 5 in this axis.
    QList<QPoint> scanLine(int startX, int startY, int dx, int dy, const QString& color);
};

#endif // SOLVER_H
