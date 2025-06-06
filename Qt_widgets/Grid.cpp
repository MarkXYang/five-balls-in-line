#include "Grid.h"
#include <QRandomGenerator> // For random number generation
#include <QDebug> // For potential debugging

Grid::Grid() {
    m_availableColors << "red" << "blue" << "green" << "yellow" << "purple" << "pink" << "brown" << "turquoise";
    initializeGrid();
}

Grid::~Grid() {
    // Clean up any remaining Ball objects
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (m_gridData[i][j] != nullptr) {
                delete m_gridData[i][j];
                m_gridData[i][j] = nullptr;
            }
        }
    }
}

void Grid::initializeGrid() {
    m_gridData.resize(GRID_SIZE);
    for (int i = 0; i < GRID_SIZE; ++i) {
        m_gridData[i].resize(GRID_SIZE);
        for (int j = 0; j < GRID_SIZE; ++j) {
            m_gridData[i][j] = nullptr;
        }
    }
    m_currentMaxBallId = 0; // Reset ball ID counter if re-initializing
}

Ball* Grid::getBallAt(int x, int y) const {
    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
        return m_gridData[x][y];
    }
    return nullptr; // Out of bounds
}

bool Grid::isCellEmpty(int x, int y) const {
    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
        return m_gridData[x][y] == nullptr;
    }
    return false; // Out of bounds is not "empty" in a usable sense
}

bool Grid::placeBall(int x, int y, Ball* ball) {
    if (ball == nullptr) return false; // Cannot place a null ball
    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
        if (isCellEmpty(x, y)) {
            m_gridData[x][y] = ball;
            return true;
        }
    }
    return false; // Cell not empty or out of bounds
}

Ball* Grid::removeBall(int x, int y) {
    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
        Ball* ball = m_gridData[x][y];
        m_gridData[x][y] = nullptr;
        return ball; // Caller is responsible for deleting this ball object if necessary
    }
    return nullptr; // Out of bounds or cell was empty
}

QList<QPoint> Grid::getEmptyCells() const {
    QList<QPoint> emptyCells;
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (m_gridData[i][j] == nullptr) {
                emptyCells.append(QPoint(i, j));
            }
        }
    }
    return emptyCells;
}

QString Grid::getRandomColor() const {
    if (m_availableColors.isEmpty()) {
        return QString(); // Should not happen if initialized correctly
    }
    int randomIndex = QRandomGenerator::global()->bounded(m_availableColors.size());
    return m_availableColors[randomIndex];
}

QPoint Grid::placeRandomBall(const QString& color) {
    QList<QPoint> emptyCells = getEmptyCells();
    if (emptyCells.isEmpty()) {
        return QPoint(-1, -1); // No space to place a ball
    }

    int randomIndex = QRandomGenerator::global()->bounded(emptyCells.size());
    QPoint randomCell = emptyCells[randomIndex];

    m_currentMaxBallId++;
    Ball* newBall = new Ball(color, m_currentMaxBallId);
    if (placeBall(randomCell.x(), randomCell.y(), newBall)) {
        return randomCell;
    }
    // Should not happen if logic is correct (empty cell was chosen)
    // but as a fallback:
    delete newBall; // Clean up if placement failed unexpectedly
    m_currentMaxBallId--; // Roll back ID
    return QPoint(-1,-1);
}

void Grid::placeInitialBalls(int count) {
    for (int i = 0; i < count; ++i) {
        QString color = getRandomColor();
        if (color.isEmpty()) { // Should not happen
            qWarning() << "Could not get random color.";
            continue;
        }
        QPoint placedPos = placeRandomBall(color);
        if (placedPos.x() < 0) { // Check for invalid point
            // Grid might be full, stop trying
            qWarning() << "Could not place initial ball, grid might be full.";
            break;
        }
    }
}

int Grid::getGridSize() const {
    return GRID_SIZE;
}

int Grid::getBallCount() const {
    int count = 0;
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (m_gridData[i][j] != nullptr) {
                count++;
            }
        }
    }
    return count;
}

QStringList Grid::getAvailableColors() const {
    return m_availableColors;
}
