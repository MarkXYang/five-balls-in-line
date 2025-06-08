#ifndef BALL_H
#define BALL_H

enum class BallColor {
    EMPTY,
    RED,
    GREEN,
    BLUE,
    YELLOW,
    PURPLE
    // Add more colors if needed
};

class Ball {
public:
    Ball(BallColor color = BallColor::EMPTY);

    BallColor getColor() const;
    bool isEmpty() const;
    void setColor(BallColor color);

private:
    BallColor m_color;
};

#endif //BALL_H
