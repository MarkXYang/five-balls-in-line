#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "GameGrid.h"
#include <vector>
#include <queue>
#include <utility> // For std::pair

class Pathfinder {
public:
    Pathfinder(const GameGrid* gameGrid);

    bool canReach(int startR, int startC, int endR, int endC);

private:
    const GameGrid* m_gameGrid;
    // Helper method for BFS if needed, or implement directly in canReach
};

#endif //PATHFINDER_H
