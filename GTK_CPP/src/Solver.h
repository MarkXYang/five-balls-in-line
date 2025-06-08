#ifndef SOLVER_H
#define SOLVER_H

#include "GameGrid.h"
#include <vector>
#include <set>
#include <utility> // For std::pair

class Solver {
public:
    Solver(const GameGrid* gameGrid);

    // Returns a vector of coordinates of all balls that are part of a line.
    // minLength is the minimum number of same-colored balls to form a line.
    std::vector<std::pair<int, int>> findLines(int minLength = 5);

private:
    const GameGrid* m_gameGrid;

    // Helper to check a specific direction from a starting cell
    void checkDirection(int r, int c, int dr, int dc, int minLength, BallColor color,
                        std::set<std::pair<int, int>>& lineCells) const;
};

#endif //SOLVER_H
