#include "Grid.h"
#include <QRandomGenerator> // For getRandomColor and placeRandomBall
#include <QDebug> // For debugging purposes

Grid::Grid() : m_currentMaxBallId(0) {
    // m_availableColors is initialized by its default constructor
    // m_gridData is initialized by its default constructor
    // m_emptyCells is initialized by its default constructor
    initializeGrid(); // This will set up m_gridData and m_emptyCells
}

Grid::~Grid() {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            delete m_gridData[i][j];
            m_gridData[i][j] = nullptr;
        }
    }
    // m_emptyCells QList of QPoint will clean itself up
}

void Grid::initializeGrid() {
    m_availableColors << "red" << "green" << "blue" << "yellow" << "purple" << "orange" << "cyan";

    m_gridData.resize(GRID_SIZE);
    m_emptyCells.clear(); // Clear before repopulating

    for (int i = 0; i < GRID_SIZE; ++i) {
        m_gridData[i].resize(GRID_SIZE); // Ensure inner vector is sized
        for (int j = 0; j < GRID_SIZE; ++j) {
            // Delete any existing ball if re-initializing
            if (m_gridData[i][j] != nullptr) {
                delete m_gridData[i][j];
            }
            m_gridData[i][j] = nullptr;
            m_emptyCells.append(QPoint(i, j)); // Add all cells as empty initially
        }
    }
    m_currentMaxBallId = 0;
}

Ball* Grid::getBallAt(int x, int y) const {
    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
        return m_gridData[x][y];
    }
    return nullptr;
}

bool Grid::isCellEmpty(int x, int y) const {
    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
        return m_gridData[x][y] == nullptr;
    }
    return false; // Out of bounds is not considered "empty" in a usable way
}

bool Grid::placeBall(int x, int y, Ball* ball) {
    if (ball == nullptr) return false; // Cannot place a null ball

    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
        if (isCellEmpty(x, y)) {
            m_gridData[x][y] = ball;
            // QPoint(x,y) must be removed from m_emptyCells.
            // It's crucial that removeOne succeeds. If it doesn't, m_emptyCells was out of sync.
            bool removed = m_emptyCells.removeOne(QPoint(x, y));
            if (!removed) {
                // This indicates an inconsistency: the cell was empty in m_gridData,
                // but QPoint(x,y) was not in m_emptyCells.
                // This should not happen if logic is correct everywhere.
                // Consider adding a warning or assertion here for debugging.
                qWarning() << "Grid::placeBall - Inconsistency: QPoint(" << x << "," << y << ") was not in m_emptyCells but cell was empty.";
            }
            return true;
        }
    }
    return false;
}

Ball* Grid::removeBall(int x, int y) {
    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
        Ball* ball = m_gridData[x][y];
        if (ball != nullptr) { // If there was a ball
            m_gridData[x][y] = nullptr;
            // QPoint(x,y) must be added to m_emptyCells.
            // Check if it's already there to prevent duplicates, though it shouldn't be.
            if (!m_emptyCells.contains(QPoint(x,y))) {
                m_emptyCells.append(QPoint(x, y));
            } else {
                // This indicates an inconsistency: the cell was occupied in m_gridData,
                // but QPoint(x,y) was already in m_emptyCells.
                // This should not happen.
                qWarning() << "Grid::removeBall - Inconsistency: QPoint(" << x << "," << y << ") was already in m_emptyCells but cell was occupied.";
            }
            return ball;
        }
        // If m_gridData[x][y] was already nullptr, do nothing to m_emptyCells.
        // It should already contain QPoint(x,y) if consistent.
        return nullptr;
    }
    return nullptr;
}

QList<QPoint> Grid::getEmptyCells() const {
    return m_emptyCells; // Return the maintained list
}

QString Grid::getRandomColor() const {
    if (m_availableColors.isEmpty()) {
        return QString("gray"); // Fallback color
    }
    int randomIndex = QRandomGenerator::global()->bounded(m_availableColors.size());
    return m_availableColors[randomIndex];
}

QPoint Grid::placeRandomBall(const QString& color) {
    if (m_emptyCells.isEmpty()) {
        return QPoint(-1, -1); // No space to place a ball
    }

    int randomIndex = QRandomGenerator::global()->bounded(m_emptyCells.size());
    QPoint randomCell = m_emptyCells[randomIndex]; // Get a cell that is supposed to be empty

    m_currentMaxBallId++;
    Ball* newBall = new Ball(color.isEmpty() ? getRandomColor() : color, m_currentMaxBallId);

    // placeBall will check if randomCell is empty and then update m_emptyCells by removing randomCell.
    if (placeBall(randomCell.x(), randomCell.y(), newBall)) {
        return randomCell;
    } else {
        // This case implies an inconsistency or an unexpected failure in placeBall.
        // If placeBall failed because the cell from m_emptyCells was NOT empty,
        // then m_emptyCells is out of sync with m_gridData.
        qWarning() << "Grid::placeRandomBall - Failed to place ball at supposedly empty cell: "
                   << randomCell.x() << "," << randomCell.y()
                   << ". This indicates an inconsistency between m_gridData and m_emptyCells.";
        // Clean up the created ball
        delete newBall;
        m_currentMaxBallId--;

        // Attempt to recover or diagnose:
        // 1. Explicitly check if the cell is empty in m_gridData
        if (!isCellEmpty(randomCell.x(), randomCell.y())) {
            qWarning() << "Cell " << randomCell.x() << "," << randomCell.y() << " was not empty in m_gridData!";
            // Try to remove it from m_emptyCells if it's there, as it's clearly not empty.
            m_emptyCells.removeOne(randomCell);
        } else {
            qWarning() << "Cell " << randomCell.x() << "," << randomCell.y() << " was empty in m_gridData, but placeBall still failed.";
        }
        // To prevent infinite loops if m_emptyCells is full of "phantom" empty cells,
        // it might be better to return an error or re-check another cell.
        // For now, just return error.
        return QPoint(-1, -1);
    }
}

void Grid::placeInitialBalls(int count) {
    for (int i = 0; i < count; ++i) {
        QString color = getRandomColor();
        QPoint placedPos = placeRandomBall(color);
        if (placedPos == QPoint(-1, -1)) {
            // qDebug() << "Could not place initial ball, grid might be full.";
            break; // Stop if grid is full
        }
    }
}

int Grid::getGridSize() const {
    return GRID_SIZE;
}

int Grid::getBallCount() const {
    // This can be optimized by: return GRID_SIZE * GRID_SIZE - m_emptyCells.size();
    // However, the original implementation iterates. For consistency with subtask,
    // I will change this to use m_emptyCells.size().
    return (GRID_SIZE * GRID_SIZE) - m_emptyCells.size();
}

QStringList Grid::getAvailableColors() const {
    return m_availableColors;
}
