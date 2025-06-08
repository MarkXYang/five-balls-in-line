#include "Ball.h"

Ball::Ball(BallColor color) : m_color(color) {
    // Constructor body can be empty if initialization list suffices
}

BallColor Ball::getColor() const {
    return m_color;
}

bool Ball::isEmpty() const {
    return m_color == BallColor::EMPTY;
}

void Ball::setColor(BallColor color) {
    m_color = color;
}
