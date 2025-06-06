#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView> // For QGraphicsView forward declaration, actual include in .cpp
#include <QGraphicsScene> // For QGraphicsScene forward declaration
#include <QLabel> // For QLabel forward declaration
#include <QMap>
#include <QString>
#include <QPixmap>
#include "Grid.h"       // Definition of Grid
#include "BallItem.h"   // Definition of BallItem
#include "Pathfinder.h" // Definition of Pathfinder
// #include <QPointer>  // QPointer is not suitable for QGraphicsItem
#include <QPropertyAnimation> // For QPropertyAnimation
#include "Solver.h"           // Definition of Solver

// Forward declarations for Qt UI classes used in the .cpp file
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; } // Only if using .ui file, which we are not currently
QT_END_NAMESPACE

class QGraphicsView;
class QGraphicsScene;
class QLabel;
class QHBoxLayout;
class QVBoxLayout;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // Ui::MainWindow *ui; // Not using .ui file for now

    QGraphicsScene* m_scene; // Scene to display balls and grid
    QGraphicsView* m_graphicsView; // View for the scene
    QLabel* m_scoreLabel;
    QLabel* m_upcomingBallLabels[3]; // For upcoming balls

    Grid m_grid; // The game grid logic
    QMap<QString, QPixmap> m_ballPixmaps; // Cache for ball images
    Pathfinder m_pathfinder; // Pathfinder instance
    Solver m_solver;         // Solver instance

    int m_score = 0;         // Player's score

    BallItem* m_selectedBallItem;          // Currently selected BallItem (raw pointer)
    QPoint m_selectedGridPos;              // Grid coordinates of the selected ball

    QPropertyAnimation* m_ballAnimation;   // For animating ball movement
    Ball* m_ballBeingMoved = nullptr;      // Pointer to the ball being animated
    QPoint m_targetMovePos;                // Target grid position for the animated ball
    bool m_isAnimating = false;            // Flag to disable clicks during animation

    QList<QString> m_upcomingBallColors;   // Stores colors for the next set of balls

    void setupUI(); // Helper to set up initial UI elements
    void loadBallPixmaps();
    void drawGrid(); // Clears scene and redraws grid cells and all balls
    void drawBall(int x, int y, Ball* ball); // Adds a single BallItem to the scene

    void highlightBallItem(BallItem* item, bool highlight); // Visual feedback for selection

    void generateUpcomingBalls(); // Generates 3 new upcoming ball colors
    void displayUpcomingBalls();  // Updates the UI to show upcoming balls
    bool checkGameOver();         // Checks if the grid is full

    static const int CELL_SIZE = 50; // Define cell size, matches scene setup

protected:
    bool eventFilter(QObject* watched, QEvent* event); // Removed override temporarily

private slots:
    void onAnimationFinished();
};
#endif // MAINWINDOW_H
