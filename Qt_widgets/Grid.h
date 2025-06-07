// Qt_widgets/Grid.h
#ifndef GRID_H
#define GRID_H

#include "Ball.h" // Ball is used directly in gridData
#include <QVector>
#include <QList>   // For m_emptyCells
#include <QPoint>
#include <QStringList>
#include <QRandomGenerator> // Required for placeRandomBall

class Grid {
public:
    Grid();
    ~Grid();

    void initializeGrid();
    Ball* getBallAt(int x, int y) const;
    bool isCellEmpty(int x, int y) const;
    bool placeBall(int x, int y, Ball* ball); // Will update m_emptyCells
    Ball* removeBall(int x, int y);           // Will update m_emptyCells
    QList<QPoint> getEmptyCells() const;      // Will return m_emptyCells
    QPoint placeRandomBall(const QString& color); // Will use m_emptyCells

    void placeInitialBalls(int count);
    int getGridSize() const;
    int getBallCount() const;
    QStringList getAvailableColors() const;
    QString getRandomColor() const; // Moved from private to public


    // Public constant for grid size
    static const int GRID_SIZE = 9;

private:
    QVector<QVector<Ball*>> m_gridData;
    QStringList m_availableColors;
    int m_currentMaxBallId;
    QList<QPoint> m_emptyCells; // Stores the coordinates of all empty cells
};

#endif // GRID_H
