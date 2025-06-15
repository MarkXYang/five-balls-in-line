#include "GameGrid.h"
#include <vector>
#include <algorithm> // For std::shuffle
#include <random>    // For std::random_device, std::uniform_int_distribution

GameGrid::GameGrid(int width, int height)
  : m_width(width),
    m_height(height),
    m_rng(std::random_device{}()) { // Initialize RNG
    m_balls.resize(m_height);
    for (int i = 0; i < m_height; ++i) {
        m_balls[i].resize(m_width, Ball(BallColor::EMPTY));
    }
}

const Ball& GameGrid::getBall(int r, int c) const {
    // Assuming r, c are valid. Add boundary checks if necessary for robustness.
    return m_balls[r][c];
}

void GameGrid::placeBall(int r, int c, BallColor color) {
    // Assuming r, c are valid.
    m_balls[r][c].setColor(color);
}

void GameGrid::removeBall(int r, int c) {
    // Assuming r, c are valid.
    m_balls[r][c].setColor(BallColor::EMPTY);
}

bool GameGrid::isCellEmpty(int r, int c) const {
    // Assuming r, c are valid.
    return m_balls[r][c].isEmpty();
}

int GameGrid::getWidth() const {
    return m_width;
}

int GameGrid::getHeight() const {
    return m_height;
}

std::vector<std::pair<int, int>> GameGrid::addRandomBalls(int count) {
    std::vector<std::pair<int, int>> emptyCells;
    for (int r = 0; r < m_height; ++r) {
        for (int c = 0; c < m_width; ++c) {
            if (m_balls[r][c].isEmpty()) {
                emptyCells.push_back({r, c});
            }
        }
    }

    std::shuffle(emptyCells.begin(), emptyCells.end(), m_rng);

    std::vector<std::pair<int, int>> addedBallsCoordinates;
    int numBallsToAdd = std::min(count, static_cast<int>(emptyCells.size()));

    // Define BallColors excluding EMPTY
    std::vector<BallColor> availableColors = {
        BallColor::RED, BallColor::GREEN, BallColor::BLUE,
        BallColor::YELLOW, BallColor::PURPLE
        // Add any other game colors here
    };
    if (availableColors.empty()) { // Should not happen in a real game setup
        return addedBallsCoordinates;
    }

    std::uniform_int_distribution<int> colorDist(0, availableColors.size() - 1);

    for (int i = 0; i < numBallsToAdd; ++i) {
        std::pair<int, int> cell = emptyCells[i];
        BallColor randomColor = availableColors[colorDist(m_rng)];
        placeBall(cell.first, cell.second, randomColor);
        addedBallsCoordinates.push_back(cell);
    }

    return addedBallsCoordinates;
}

bool GameGrid::isFull() const {
    for (int r = 0; r < m_height; ++r) {
        for (int c = 0; c < m_width; ++c) {
            if (m_balls[r][c].isEmpty()) {
                return false; // Found an empty cell
            }
        }
    }
    return true; // No empty cells found
}

void GameGrid::reset() {
    for (int r = 0; r < m_height; ++r) {
        for (int c = 0; c < m_width; ++c) {
            // Assuming Ball has a constructor Ball(BallColor::EMPTY) or a setColor method
            m_balls[r][c].setColor(BallColor::EMPTY);
        }
    }
}
