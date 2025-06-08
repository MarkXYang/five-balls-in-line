#ifndef GAMEGRID_H
#define GAMEGRID_H

#include "Ball.h"
#include <vector>
#include <random> // For std::mt19937 and std::random_device
#include <utility> // For std::pair

class GameGrid {
public:
    GameGrid(int width = 9, int height = 9);

    const Ball& getBall(int r, int c) const;
    // Mutable version to allow direct modification if needed, e.g. m_balls[r][c].setColor()
    // Ball& getBall(int r, int c); // Decided against this to enforce using placeBall/removeBall
    void placeBall(int r, int c, BallColor color);
    void removeBall(int r, int c);
    bool isCellEmpty(int r, int c) const;

    int getWidth() const;
    int getHeight() const;

    std::vector<std::pair<int, int>> addRandomBalls(int count);
    bool isFull() const;
    void reset(); // Added reset method

private:
    int m_width;
    int m_height;
    std::vector<std::vector<Ball>> m_balls;

    std::mt19937 m_rng; // Mersenne Twister engine for random numbers
};

#endif //GAMEGRID_H
