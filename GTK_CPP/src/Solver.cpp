#include "Solver.h"
#include <vector>
#include <set>
#include <algorithm> // For std::sort if needed, not strictly for set to vector

Solver::Solver(const GameGrid* gameGrid) : m_gameGrid(gameGrid) {}

// Helper function for findLines
void Solver::checkDirection(int r, int c, int dr, int dc, int minLength, BallColor color,
                            std::set<std::pair<int, int>>& lineCells) const {
    if (!m_gameGrid || color == BallColor::EMPTY) {
        return;
    }

    std::vector<std::pair<int, int>> currentLine;
    currentLine.push_back({r, c});

    int currentR = r + dr;
    int currentC = c + dc;

    while (currentR >= 0 && currentR < m_gameGrid->getHeight() &&
           currentC >= 0 && currentC < m_gameGrid->getWidth() &&
           m_gameGrid->getBall(currentR, currentC).getColor() == color) {
        currentLine.push_back({currentR, currentC});
        currentR += dr;
        currentC += dc;
    }

    if (currentLine.size() >= static_cast<size_t>(minLength)) {
        for (const auto& cell : currentLine) {
            lineCells.insert(cell);
        }
    }
}


std::vector<std::pair<int, int>> Solver::findLines(int minLength) {
    std::set<std::pair<int, int>> lineCellsSet; // Use a set to automatically handle duplicates

    if (!m_gameGrid) {
        return {}; // Return empty vector if no grid
    }

    int height = m_gameGrid->getHeight();
    int width = m_gameGrid->getWidth();

    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            const Ball& ball = m_gameGrid->getBall(r, c);
            BallColor color = ball.getColor();

            if (color != BallColor::EMPTY) {
                // Check Horizontal: (0, 1)
                checkDirection(r, c, 0, 1, minLength, color, lineCellsSet);
                // Check Vertical: (1, 0)
                checkDirection(r, c, 1, 0, minLength, color, lineCellsSet);
                // Check Diagonal Down-Right: (1, 1)
                checkDirection(r, c, 1, 1, minLength, color, lineCellsSet);
                // Check Diagonal Up-Right (or Down-Left from the perspective of the top-leftmost ball in such a line): (1, -1)
                // For a line like:
                //  B
                // B
                //B
                // We check from (r,c) with dr=1, dc=-1. This finds lines going "down-left".
                // To find lines going "up-right", we can check with dr=-1, dc=1.
                // However, a simpler way to cover all lines is to only check directions that "originate" from (r,c)
                // (i.e. right, down, down-right, down-left). An "up-right" line will be found when its
                // bottom-leftmost ball is (r,c) and we check (dr=-1, dc=1).
                // The current checkDirection will find a segment starting from (r,c).
                // To ensure full lines are found, not just segments starting from (r,c) going one way:
                // The current findLines iterates (r,c) over all cells. If a line exists, it will be found
                // when (r,c) is one of the balls in that line. The checkDirection will then find all balls
                // in that direction from (r,c).
                // The problem is that checkDirection only checks in *one* direction.
                // A line is e.g. B-B-B-B-B. If (r,c) is the 2nd B, checkDirection(0,1) finds B-B-B-B.
                // This is fine because the set will combine overlapping segments.

                // The prompt description implies checking 4 directions.
                // Horizontal (->) : (dr=0, dc=1) - Covered
                // Vertical (v) : (dr=1, dc=0) - Covered
                // Diagonal down-right (\) : (dr=1, dc=1) - Covered
                // Diagonal up-right (/) : (dr=-1, dc=1)
                // This one is needed if we only start checks from each ball.
                // Example:  X X B X X
                //           X B X X X
                //           B X X X X
                // If (r,c) is the bottom-left B, (dr=-1, dc=1) will find the line.
                checkDirection(r, c, -1, 1, minLength, color, lineCellsSet);
            }
        }
    }

    // Convert set to vector
    std::vector<std::pair<int, int>> linesVector(lineCellsSet.begin(), lineCellsSet.end());
    return linesVector;
}
