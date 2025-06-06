#include "Ball.h"

Ball::Ball(const QString& color, int id)
    : m_color(color), m_id(id) {
    // Constructor implementation
    // Initialization is done in the member initializer list
}

QString Ball::getColor() const {
    return m_color;
}

int Ball::getId() const {
    return m_id;
}

// If we were to add more methods, they would be implemented here.
