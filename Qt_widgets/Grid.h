#ifndef GRID_H
#define GRID_H

#include <QVector>
#include <QPoint>
#include <QStringList>
#include "Ball.h" // Assuming Ball.h is in the same directory

class Grid {
public:
    static const int GRID_SIZE = 9;

    Grid();
    ~Grid(); // Destructor to clean up Ball objects

    void initializeGrid();
    Ball* getBallAt(int x, int y) const;
    bool isCellEmpty(int x, int y) const;
    bool placeBall(int x, int y, Ball* ball);
    Ball* removeBall(int x, int y);
    QList<QPoint> getEmptyCells() const;
    QPoint placeRandomBall(const QString& color); // Returns QPoint of placement or (-1,-1)
    void placeInitialBalls(int count);
    QString getRandomColor() const;
    int getGridSize() const;
    int getBallCount() const; // Useful for game logic/scoring
    QStringList getAvailableColors() const; // Getter for available colors

private:
    QVector<QVector<Ball*>> m_gridData;
    int m_currentMaxBallId = 0; // To generate unique IDs for balls
    QStringList m_availableColors;
};

#endif // GRID_H
