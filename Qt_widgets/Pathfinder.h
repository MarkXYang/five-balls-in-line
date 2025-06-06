#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <QVector>
#include <QPoint>
#include <QList> // For the path result
#include <QSet>  // For the closed list

#include <QtCore/qhashfunctions.h> // For qHash generic implementations (needed for the custom qHash)
#include <QtGlobal>                // For Q_ASSERT and other Qt globals (if used in qHash)

// Forward declaration
class Grid;

// qHash for QPoint is now in "qtpoint_hash.h"

namespace Pathfinding { // Encapsulate Node to avoid global namespace pollution

struct Node {
    QPoint position;
    int gCost;      // Cost from start to this node
    int hCost;      // Heuristic cost from this node to end
    Node* parent;   // Pointer to the parent node for path reconstruction

    Node(QPoint pos, int g, int h, Node* p)
        : position(pos), gCost(g), hCost(h), parent(p) {}

    int fCost() const { return gCost + hCost; }

    // For priority queue if std::priority_queue is used with custom objects
    // bool operator>(const Node& other) const {
    //     return fCost() > other.fCost(); // Min-heap behavior
    // }
};

// Custom comparator for std::sort or a min-heap if not using std::priority_queue directly
// with operator>
struct CompareNode {
    bool operator()(const Node* a, const Node* b) const {
        if (a->fCost() == b->fCost()) {
            return a->hCost > b->hCost; // Tie-breaking: prefer smaller hCost
        }
        return a->fCost() > b->fCost(); // Min-heap behavior (lowest fCost has higher priority)
    }
};

} // namespace Pathfinding


class Pathfinder {
public:
    Pathfinder(const Grid* grid); // Constructor takes a const pointer to the grid

    // Finds a path from start to end. Returns an empty list if no path is found.
    QList<QPoint> findPath(const QPoint& start, const QPoint& end);

private:
    const Grid* m_grid; // Pointer to the grid, Pathfinder does not own it

    // Calculates the heuristic (Manhattan distance) between two points
    int calculateHeuristic(const QPoint& a, const QPoint& b) const;

    // Reconstructs the path from the target node back to the start
    QList<QPoint> reconstructPath(Pathfinding::Node* targetNode) const;
};

#endif // PATHFINDER_H
