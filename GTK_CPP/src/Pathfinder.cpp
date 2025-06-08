#include "Pathfinder.h"
#include <vector>
#include <queue>
#include <set> // For visited set

Pathfinder::Pathfinder(const GameGrid* gameGrid) : m_gameGrid(gameGrid) {}

bool Pathfinder::canReach(int startR, int startC, int endR, int endC) {
    if (!m_gameGrid) {
        return false;
    }
    if (startR == endR && startC == endC) {
        return true; // Already at the destination
    }

    int height = m_gameGrid->getHeight();
    int width = m_gameGrid->getWidth();

    // Boundary checks for start and end points
    if (startR < 0 || startR >= height || startC < 0 || startC >= width ||
        endR < 0 || endR >= height || endC < 0 || endC >= width) {
        return false;
    }

    // BFS implementation
    std::queue<std::pair<int, int>> q;
    std::set<std::pair<int, int>> visited; // To keep track of visited cells

    q.push({startR, startC});
    visited.insert({startR, startC});

    // Directions: up, down, left, right
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    while (!q.empty()) {
        std::pair<int, int> current = q.front();
        q.pop();

        int r = current.first;
        int c = current.second;

        for (int i = 0; i < 4; ++i) {
            int nextR = r + dr[i];
            int nextC = c + dc[i];

            if (nextR == endR && nextC == endC) {
                // Destination must be empty to be reachable by path definition
                // But the problem states "path of empty cells from start to end"
                // The end cell itself is where we place the ball, so it must be empty *before* moving.
                // The GameGrid::isCellEmpty(endR, endC) is checked in onCellClicked
                // So, Pathfinder just needs to find a path of empty cells up to the destination.
                return true;
            }

            // Check boundaries
            if (nextR >= 0 && nextR < height && nextC >= 0 && nextC < width) {
                // Check if the cell is empty and not visited
                if (m_gameGrid->isCellEmpty(nextR, nextC) && visited.find({nextR, nextC}) == visited.end()) {
                    visited.insert({nextR, nextC});
                    q.push({nextR, nextC});
                }
            }
        }
    }

    return false; // Destination not reached
}
