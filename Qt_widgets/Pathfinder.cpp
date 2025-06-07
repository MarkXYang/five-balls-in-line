// Qt_widgets/Pathfinder.cpp
#include "Pathfinder.h"
#include "Grid.h"
#include "qtpoint_hash.h"
#include <QSet>
#include <QPoint>
#include <QtCore/qhashfunctions.h>
#include <QtGlobal>
#include <cmath>
#include <queue>    // Required for std::priority_queue
#include <vector>   // Required for std::vector (used by std::priority_queue)
// No longer need <algorithm> for std::sort or std::find_if on openList
// No longer need <QVector> for openList specifically

// Pathfinding namespace content (Node, CompareNode) is now in Pathfinder.h

Pathfinder::Pathfinder(const Grid* grid) : m_grid(grid) {
    Q_ASSERT(m_grid != nullptr);
}

int Pathfinder::calculateHeuristic(const QPoint& a, const QPoint& b) const {
    return std::abs(a.x() - b.x()) + std::abs(a.y() - b.y()); // Manhattan distance
}

QList<QPoint> Pathfinder::reconstructPath(Pathfinding::Node* targetNode) const {
    QList<QPoint> path;
    Pathfinding::Node* currentNode = targetNode;
    while (currentNode != nullptr) {
        path.prepend(currentNode->position);
        currentNode = currentNode->parent;
    }
    return path;
}

QList<QPoint> Pathfinder::findPath(const QPoint& start, const QPoint& end) {
    if (!m_grid) {
        return QList<QPoint>();
    }

    if (start == end) {
        return QList<QPoint>();
    }

    // Check bounds for start and end points
    if (start.x() < 0 || start.x() >= Grid::GRID_SIZE || start.y() < 0 || start.y() >= Grid::GRID_SIZE ||
        end.x() < 0 || end.x() >= Grid::GRID_SIZE || end.y() < 0 || end.y() >= Grid::GRID_SIZE) {
        return QList<QPoint>();
    }

    if (!m_grid->isCellEmpty(end.x(), end.y())) {
        return QList<QPoint>();
    }

    std::priority_queue<Pathfinding::Node*, std::vector<Pathfinding::Node*>, Pathfinding::CompareNode> openList;
    QSet<QPoint> closedList;
    QList<Pathfinding::Node*> allNodes; // To manage memory deletion

    Pathfinding::Node* startNode = new Pathfinding::Node(start, 0, calculateHeuristic(start, end), nullptr);
    allNodes.append(startNode);
    openList.push(startNode);

    int D = 1; // Cost for adjacent (non-diagonal) movement

    while (!openList.empty()) {
        Pathfinding::Node* currentNode = openList.top();
        openList.pop();

        if (closedList.contains(currentNode->position)) {
            continue;
        }

        if (currentNode->position == end) {
            QList<QPoint> path = reconstructPath(currentNode);
            qDeleteAll(allNodes);
            return path;
        }

        closedList.insert(currentNode->position);

        QPoint neighbors[4] = {
            QPoint(currentNode->position.x(), currentNode->position.y() - 1), // Up
            QPoint(currentNode->position.x(), currentNode->position.y() + 1), // Down
            QPoint(currentNode->position.x() - 1, currentNode->position.y()), // Left
            QPoint(currentNode->position.x() + 1, currentNode->position.y())  // Right
        };

        for (const QPoint& neighborPos : neighbors) {
            if (neighborPos.x() < 0 || neighborPos.x() >= Grid::GRID_SIZE ||
                neighborPos.y() < 0 || neighborPos.y() >= Grid::GRID_SIZE) {
                continue;
            }

            if (closedList.contains(neighborPos)) {
                continue;
            }

            if (!m_grid->isCellEmpty(neighborPos.x(), neighborPos.y()) && neighborPos != end) {
                 continue;
            }

            int tentativeGCost = currentNode->gCost + D;

            Pathfinding::Node* neighborNode = new Pathfinding::Node(
                neighborPos,
                tentativeGCost,
                calculateHeuristic(neighborPos, end),
                currentNode
            );
            allNodes.append(neighborNode);
            openList.push(neighborNode);
        }
    }

    qDeleteAll(allNodes);
    return QList<QPoint>();
}
