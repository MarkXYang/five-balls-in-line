// Qt_widgets/Pathfinder.h
#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <QList>
#include <QPoint>
#include <vector>   // Required for std::vector (used by std::priority_queue)
#include <queue>    // Required for std::priority_queue

// Forward declaration
class Grid;

namespace Pathfinding {
    struct Node {
        QPoint position;
        int gCost;
        int hCost;
        Node* parent;

        Node(QPoint pos, int g, int h, Node* p)
            : position(pos), gCost(g), hCost(h), parent(p) {}

        int fCost() const { return gCost + hCost; }
    };

    // Comparator for the priority queue (min-heap for fCost)
    struct CompareNode {
        bool operator()(const Node* a, const Node* b) const {
            if (a->fCost() == b->fCost()) {
                return a->hCost > b->hCost; // Tie-breaking: prefer smaller hCost
            }
            return a->fCost() > b->fCost();
        }
    };
} // namespace Pathfinding

class Pathfinder {
public:
    Pathfinder(const Grid* grid);
    QList<QPoint> findPath(const QPoint& start, const QPoint& end);

private:
    const Grid* m_grid;

    int calculateHeuristic(const QPoint& a, const QPoint& b) const;
    QList<QPoint> reconstructPath(Pathfinding::Node* targetNode) const;
};

#endif // PATHFINDER_H
