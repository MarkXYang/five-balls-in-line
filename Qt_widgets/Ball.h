#ifndef BALL_H
#define BALL_H

#include <QString>
// QColor might be needed later if we want to draw balls programmatically
// #include <QColor>

class Ball {
public:
    Ball(const QString& color, int id);

    QString getColor() const;
    int getId() const;

    // Optional: if direct member access is preferred, make them public
    // QString color;
    // int id;

private:
    QString m_color;
    int m_id;
};

#endif // BALL_H
