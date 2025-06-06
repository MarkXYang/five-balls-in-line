#ifndef QTPOINT_HASH_H
#define QTPOINT_HASH_H

#include <QPoint> // For QPoint itself
#include <QtCore/qhashfunctions.h> // For the underlying qHash(int, uint)

// Custom qHash for QPoint to be used with QSet<QPoint>, QHash<QPoint, ...> etc.
// Must be inline if defined in a header file to prevent multiple definition errors.
inline uint qHash(const QPoint& point, uint seed = 0) noexcept {
    // A common way to combine hashes is to XOR them,
    // potentially with a shift or multiplication for one of the components
    // to ensure (1,2) and (2,1) produce different hash values more reliably.
    return qHash(point.x(), seed) ^ qHash(point.y(), seed << 1);
}

#endif // QTPOINT_HASH_H
