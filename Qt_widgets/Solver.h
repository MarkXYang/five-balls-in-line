// Qt_widgets/Solver.h
#ifndef SOLVER_H
#define SOLVER_H

#include <QList>
#include <QPoint>
#include <QString>

class Grid; // Forward declaration

class Solver {
public:
    Solver(Grid* grid);

    // Checks for lines of 5 or more balls of the same color around the last moved ball.
    // Returns a list of unique points belonging to any such lines.
    QList<QPoint> checkForLines(int lastMovedX, int lastMovedY);

private:
    Grid* m_grid;

    // Scans in a single direction (dx, dy) from (startX, startY) for balls of a specific color.
    // Includes the ball at (startX, startY) if it matches the color.
    QList<QPoint> scanLineDirectional(int startX, int startY, int dx, int dy, const QString& color);
};

#endif // SOLVER_H
