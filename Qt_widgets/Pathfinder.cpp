#include "Pathfinder.h"
#include "Grid.h" // For Grid::isCellEmpty, Grid::GRID_SIZE
#include "qtpoint_hash.h" // For qHash(QPoint)
#include <QSet>
#include <QVector>
#include <QPoint> // Ensure QPoint is fully defined (though likely pulled by other headers)
#include <QtCore/qhashfunctions.h> // For qHash generic implementations
#include <QtGlobal> // For Q_ASSERT and other Qt globals
#include <algorithm> // For std::sort and std::find_if
#include <cmath>     // For std::abs

// Pathfinder Constructor
Pathfinder::Pathfinder(const Grid* grid) : m_grid(grid) {
    Q_ASSERT(m_grid != nullptr); // Ensure grid is not null
}

// Heuristic Calculation (Manhattan Distance)
int Pathfinder::calculateHeuristic(const QPoint& a, const QPoint& b) const {
    return std::abs(a.x() - b.x()) + std::abs(a.y() - b.y());
}

// Path Reconstruction
QList<QPoint> Pathfinder::reconstructPath(Pathfinding::Node* targetNode) const {
    QList<QPoint> path;
    Pathfinding::Node* currentNode = targetNode;
    while (currentNode != nullptr) {
        path.prepend(currentNode->position); // Add to the beginning to reverse the path
        currentNode = currentNode->parent;
    }
    return path;
}

// A* Pathfinding Algorithm
QList<QPoint> Pathfinder::findPath(const QPoint& start, const QPoint& end) {
    if (!m_grid || start == end) {
        return QList<QPoint>(); // No grid or start is end
    }

    // Check if start or end are outside grid boundaries
    if (start.x() < 0 || start.x() >= Grid::GRID_SIZE || start.y() < 0 || start.y() >= Grid::GRID_SIZE ||
        end.x() < 0 || end.x() >= Grid::GRID_SIZE || end.y() < 0 || end.y() >= Grid::GRID_SIZE) {
        return QList<QPoint>(); // Start or end out of bounds
    }

    // The target cell MUST be empty for a valid path in this game.
    // (Unlike some A* where target might be an enemy, here it's a destination cell)
    if (!m_grid->isCellEmpty(end.x(), end.y())) {
         // If the start point is the same as the end point, and the end point is occupied,
         // this implies we are trying to "move" a ball to its own location, which is not a valid path.
         // However, if start != end and end is occupied, it's simply not a valid destination.
        if (start != end) { // Only return empty if we're not just checking the start point itself
             return QList<QPoint>();
        } else if (start == end && !m_grid->isCellEmpty(start.x(), start.y())) {
            // Technically, a path to itself if it's occupied is not possible.
            // But the problem states "OR it's the end point itself...".
            // For this game, the target cell must be empty.
            // The only exception could be if we are checking for paths *from* an occupied cell,
            // but the path itself must consist of empty cells.
            // Let's stick to: target cell (end) must be empty.
             return QList<QPoint>();
        }
    }


    QVector<Pathfinding::Node*> openList;
    QSet<QPoint> closedList; // Stores QPoint for efficient lookup
    QList<Pathfinding::Node*> allNodes; // To manage memory deletion

    Pathfinding::Node* startNode = new Pathfinding::Node(start, 0, calculateHeuristic(start, end), nullptr);
    allNodes.append(startNode);
    openList.append(startNode);

    int D = 1; // Cost for adjacent (non-diagonal) movement

    while (!openList.isEmpty()) {
        // Find node with the lowest fCost in openList
        // Sort openList to bring the node with the smallest fCost to the end for easy pop_back
        // Using custom lambda for sort or std::sort with a custom comparator struct
        std::sort(openList.begin(), openList.end(), [](const Pathfinding::Node* a, const Pathfinding::Node* b){
            return Pathfinding::CompareNode()(a,b); // Uses the > operator for min-heap like behavior (smallest is last)
        });

        Pathfinding::Node* currentNode = openList.takeLast(); // Smallest fCost node

        if (currentNode->position == end) {
            QList<QPoint> path = reconstructPath(currentNode);
            qDeleteAll(allNodes); // Clean up all allocated nodes
            return path;
        }

        closedList.insert(currentNode->position);

        // Explore neighbors (Up, Down, Left, Right)
        QPoint neighbors[4] = {
            QPoint(currentNode->position.x(), currentNode->position.y() - 1), // Up
            QPoint(currentNode->position.x(), currentNode->position.y() + 1), // Down
            QPoint(currentNode->position.x() - 1, currentNode->position.y()), // Left
            QPoint(currentNode->position.x() + 1, currentNode->position.y())  // Right
        };

        for (const QPoint& neighborPos : neighbors) {
            // Check bounds
            if (neighborPos.x() < 0 || neighborPos.x() >= Grid::GRID_SIZE ||
                neighborPos.y() < 0 || neighborPos.y() >= Grid::GRID_SIZE) {
                continue;
            }

            // Check if already evaluated
            if (closedList.contains(neighborPos)) {
                continue;
            }

            // Check if walkable (empty cell)
            // The target cell (end) must be empty. All intermediate cells must also be empty.
            if (!m_grid->isCellEmpty(neighborPos.x(), neighborPos.y()) && neighborPos != end) {
                 continue;
            }
            // If the neighbor IS the end point, it must be empty (checked at the start of the function).
            // So no special handling for `neighborPos == end` regarding walkability is needed here,
            // as `isCellEmpty(end.x(), end.y())` must be true.

            int tentativeGCost = currentNode->gCost + D; // D is 1 for cardinal moves

            Pathfinding::Node* neighborNode = nullptr;
            auto it = std::find_if(openList.begin(), openList.end(), [&](Pathfinding::Node* node){
                return node->position == neighborPos;
            });

            if (it != openList.end()) { // If neighbor is in openList
                neighborNode = *it;
                if (tentativeGCost < neighborNode->gCost) {
                    neighborNode->gCost = tentativeGCost;
                    neighborNode->parent = currentNode;
                    // No need to update hCost as it's fixed for the node position
                    // openList will be re-sorted
                }
            } else { // Neighbor not in openList
                neighborNode = new Pathfinding::Node(
                    neighborPos,
                    tentativeGCost,
                    calculateHeuristic(neighborPos, end),
                    currentNode
                );
                allNodes.append(neighborNode);
                openList.append(neighborNode);
            }
        }
    }

    qDeleteAll(allNodes); // Clean up if no path found
    return QList<QPoint>(); // No path found
}
