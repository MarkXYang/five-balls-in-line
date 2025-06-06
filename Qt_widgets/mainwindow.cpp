#include "mainwindow.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QPen>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox> // For QMessageBox
#include "qtpoint_hash.h" // For qHash(QPoint)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_scene(new QGraphicsScene(this)),
      m_grid(),
      m_pathfinder(&m_grid),
      m_solver(&m_grid), // Initialize m_solver with address of m_grid
      m_selectedBallItem(nullptr),
      m_ballAnimation(new QPropertyAnimation(this)) // Parent animation to this for auto-cleanup
      // m_score is initialized to 0 by default member initialization in .h
{
    setupUI(); // Sets up m_graphicsView among other things
    loadBallPixmaps();

    m_grid.initializeGrid();
    m_grid.placeInitialBalls(5);

    drawGrid();

    setWindowTitle("Five Balls Game");
    // Adjusted initial size to better fit common screen resolutions if needed
    resize(m_graphicsView->width() + 250, m_graphicsView->height() + 150);


    // Install event filter on the scene
    m_scene->installEventFilter(this);

    // Setup ball animation properties
    m_ballAnimation->setTargetObject(nullptr); // Target will be set dynamically
    m_ballAnimation->setPropertyName("pos");   // Animate QGraphicsItem::pos() property
    m_ballAnimation->setDuration(300);        // 0.3 second animation duration
    connect(m_ballAnimation, &QPropertyAnimation::finished, this, &MainWindow::onAnimationFinished);

    // Initialize and display first set of upcoming balls
    generateUpcomingBalls();
    displayUpcomingBalls();
}

MainWindow::~MainWindow()
{
    // m_scene is child of MainWindow, Qt handles its deletion.
    // m_ballPixmaps, m_grid, m_pathfinder are members, destructors called.
    // m_ballAnimation is child of MainWindow, Qt handles its deletion.
    // m_selectedBallItem is QPointer, handles itself.
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget); // Set central widget for QMainWindow

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *topPanelLayout = new QHBoxLayout();
    m_scoreLabel = new QLabel("Score: 0", this);
    topPanelLayout->addWidget(m_scoreLabel);
    topPanelLayout->addStretch();

    QLabel *upcomingTitle = new QLabel("Upcoming:", this);
    topPanelLayout->addWidget(upcomingTitle);
    for (int i = 0; i < 3; ++i) {
        m_upcomingBallLabels[i] = new QLabel(QString("B%1").arg(i+1), this);
        m_upcomingBallLabels[i]->setFixedSize(CELL_SIZE, CELL_SIZE);
        m_upcomingBallLabels[i]->setAlignment(Qt::AlignCenter);
        m_upcomingBallLabels[i]->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        topPanelLayout->addWidget(m_upcomingBallLabels[i]);
    }
    mainLayout->addLayout(topPanelLayout);

    m_graphicsView = new QGraphicsView(this); // m_graphicsView is now initialized
    m_scene->setSceneRect(0, 0, Grid::GRID_SIZE * CELL_SIZE, Grid::GRID_SIZE * CELL_SIZE);
    m_graphicsView->setScene(m_scene);
    m_graphicsView->setFixedSize(Grid::GRID_SIZE * CELL_SIZE + 2 * m_graphicsView->frameWidth(),
                                 Grid::GRID_SIZE * CELL_SIZE + 2 * m_graphicsView->frameWidth());
    m_graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_graphicsView->setRenderHint(QPainter::Antialiasing); // Smoother ball rendering

    mainLayout->addWidget(m_graphicsView, 0, Qt::AlignCenter);
}


void MainWindow::loadBallPixmaps() {
    QStringList colors = m_grid.getAvailableColors();
    if (colors.isEmpty()) {
         colors << "red" << "blue" << "green" << "yellow" << "purple" << "pink" << "brown" << "turquoise";
    }

    for (const QString& color : colors) {
        QString path = QString(":/images/%1_ball.png").arg(color);
        QPixmap pixmap(path);
        if (pixmap.isNull()) {
            qWarning() << "Failed to load ball pixmap for color:" << color << "from path:" << path;
        } else {
            m_ballPixmaps[color] = pixmap.scaled(CELL_SIZE, CELL_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }
}

void MainWindow::drawGrid() {
    m_scene->clear(); // This also deletes BallItem objects, which is fine as they don't own Ball*

    for (int i = 0; i < Grid::GRID_SIZE; ++i) {
        for (int j = 0; j < Grid::GRID_SIZE; ++j) {
            QGraphicsRectItem* cellRect = new QGraphicsRectItem(i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE);
            cellRect->setBrush(Qt::lightGray);
            cellRect->setPen(QPen(Qt::darkGray));
            m_scene->addItem(cellRect);
        }
    }

    for (int i = 0; i < Grid::GRID_SIZE; ++i) {
        for (int j = 0; j < Grid::GRID_SIZE; ++j) {
            Ball* ball = m_grid.getBallAt(i, j);
            if (ball) {
                drawBall(i, j, ball);
            }
        }
    }
}

void MainWindow::drawBall(int x, int y, Ball* ball) {
    if (!ball) return;
    QString color = ball->getColor();
    if (m_ballPixmaps.contains(color)) {
        const QPixmap& pixmap = m_ballPixmaps[color];
        BallItem* ballItem = new BallItem(ball, pixmap);
        ballItem->setPos(x * CELL_SIZE, y * CELL_SIZE);
        ballItem->setZValue(0); // Default Z value
        ballItem->setScale(1.0); // Default scale
        ballItem->setOpacity(1.0); // Default opacity
        m_scene->addItem(ballItem);
    } else {
        qWarning() << "No pixmap found for color:" << color;
        QGraphicsEllipseItem* placeholder = new QGraphicsEllipseItem(x * CELL_SIZE + 5, y * CELL_SIZE + 5, CELL_SIZE - 10, CELL_SIZE - 10);
        placeholder->setBrush(Qt::magenta);
        m_scene->addItem(placeholder);
    }
}

void MainWindow::highlightBallItem(BallItem* item, bool highlight) {
    if (!item) return;
    if (highlight) {
        item->setOpacity(0.7);
        item->setScale(1.1);
        item->setZValue(1); // Bring to front
    } else {
        item->setOpacity(1.0);
        item->setScale(1.0);
        item->setZValue(0); // Reset Z value
    }
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_scene && event->type() == QEvent::GraphicsSceneMousePress && !m_isAnimating) {
        QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
        QPointF scenePos = mouseEvent->scenePos();

        int gridX = static_cast<int>(scenePos.x()) / CELL_SIZE;
        int gridY = static_cast<int>(scenePos.y()) / CELL_SIZE;

        if (gridX < 0 || gridX >= Grid::GRID_SIZE || gridY < 0 || gridY >= Grid::GRID_SIZE) {
            return QObject::eventFilter(watched, event); // Click outside grid
        }
        QPoint clickedGridPos(gridX, gridY);

        QGraphicsItem* clickedItemRaw = m_scene->itemAt(scenePos, m_graphicsView->transform());
        BallItem* clickedBallItem = dynamic_cast<BallItem*>(clickedItemRaw);

        if (!m_selectedBallItem) { // No ball selected yet
            if (clickedBallItem) { // Clicked on a ball
                m_selectedBallItem = clickedBallItem;
                m_selectedGridPos = clickedGridPos;
                highlightBallItem(m_selectedBallItem, true);
            }
        } else { // A ball is already selected
            if (m_selectedBallItem == clickedBallItem) { // Clicked the same ball again
                highlightBallItem(m_selectedBallItem, false); // Deselect
                m_selectedBallItem = nullptr;
            } else if (clickedBallItem) { // Clicked on another ball
                highlightBallItem(m_selectedBallItem, false); // Deselect old
                m_selectedBallItem = clickedBallItem;
                m_selectedGridPos = clickedGridPos;
                highlightBallItem(m_selectedBallItem, true);  // Select new
            } else { // Clicked on an empty cell (since clickedBallItem is null)
                if (m_grid.isCellEmpty(gridX, gridY)) {
                    QList<QPoint> path = m_pathfinder.findPath(m_selectedGridPos, clickedGridPos);
                    if (!path.isEmpty()) {
                        m_ballBeingMoved = m_selectedBallItem->getBall();
                        m_targetMovePos = clickedGridPos;

                        highlightBallItem(m_selectedBallItem, false); // Remove selection highlight before animation

                        // For multi-step animation:
                        // QSequentialAnimationGroup* group = new QSequentialAnimationGroup(this);
                        // BallItem* itemToAnimate = m_selectedBallItem.data(); // Keep a strong ref if QPointer clears early
                        // QPoint currentAnimPos = m_selectedGridPos;
                        // for(const QPoint& stepPos : path) {
                        //    if (stepPos == m_selectedGridPos) continue; // Skip start point
                        //    QPropertyAnimation* stepAnim = new QPropertyAnimation(itemToAnimate, "pos");
                        //    stepAnim->setEndValue(QPointF(stepPos.x() * CELL_SIZE, stepPos.y() * CELL_SIZE));
                        //    stepAnim->setDuration(150); // Duration for each step
                        //    group->addAnimation(stepAnim);
                        //    currentAnimPos = stepPos;
                        // }
                        // m_ballAnimation = group; // replace member if it's a group
                        // group->start(QAbstractAnimation::DeleteWhenStopped);
                        // m_isAnimating = true;

                        // Simple direct animation:
                        m_ballAnimation->setTargetObject(m_selectedBallItem); // Now a raw pointer
                        m_ballAnimation->setEndValue(QPointF(clickedGridPos.x() * CELL_SIZE, clickedGridPos.y() * CELL_SIZE));
                        m_ballAnimation->start();
                        m_isAnimating = true;

                        // The m_selectedBallItem will be cleared in onAnimationFinished
                        // No, we need to keep m_selectedBallItem for onAnimationFinished to update grid
                        // but m_ballBeingMoved and m_targetMovePos are what's critical
                    } else { // No path
                        highlightBallItem(m_selectedBallItem, false);
                        m_selectedBallItem = nullptr; // Deselect
                    }
                } else { // Clicked on a non-empty cell that is not a ball (should not happen)
                    highlightBallItem(m_selectedBallItem, false);
                    m_selectedBallItem = nullptr; // Deselect
                }
            }
        }
        return true; // Event handled
    }
    return QObject::eventFilter(watched, event); // Pass on other events
}

void MainWindow::onAnimationFinished() {
    m_isAnimating = false; // Re-enable clicks

    if (m_ballBeingMoved && m_selectedBallItem) { // Check if animation was for a valid move
        // Update grid logic
        m_grid.removeBall(m_selectedGridPos.x(), m_selectedGridPos.y());
        m_grid.placeBall(m_targetMovePos.x(), m_targetMovePos.y(), m_ballBeingMoved);

        // m_selectedBallItem's position is already updated by the animation.
        // No need to delete m_selectedBallItem graphics object explicitly if drawGrid() is called.
        // If not calling drawGrid(), one would need to update the BallItem's associated Ball pointer
        // if a new BallItem is created for the target cell, or simply update its grid coords.
    }

    // Clear selection state related to the move
    m_ballBeingMoved = nullptr; // Clear the ball that was being moved
    bool playerMadeLine = false;

    // 1. Check if the player's move resulted in a line
    if (m_targetMovePos.x() >= 0 && m_targetMovePos.x() < Grid::GRID_SIZE &&
        m_targetMovePos.y() >= 0 && m_targetMovePos.y() < Grid::GRID_SIZE) {

        QList<QPoint> clearedPlayerPositions = m_solver.checkForLines(m_targetMovePos.x(), m_targetMovePos.y());

        if (!clearedPlayerPositions.isEmpty()) {
            playerMadeLine = true;
            for (const QPoint& pos : clearedPlayerPositions) {
                Ball* ballToRemove = m_grid.removeBall(pos.x(), pos.y());
                if (ballToRemove) delete ballToRemove;
            }
            int baseScore = clearedPlayerPositions.size() * 2;
            if (clearedPlayerPositions.size() >= 5) baseScore += (clearedPlayerPositions.size() - 4) * clearedPlayerPositions.size();
            m_score += baseScore;
            m_scoreLabel->setText(QString("Score: %1").arg(m_score));
        }
    } else {
        qWarning() << "onAnimationFinished called with invalid m_targetMovePos, or no move was made.";
    }

    // 2. If player's move did NOT make a line, add upcoming balls and check for lines again
    if (!playerMadeLine) {
        QList<QPoint> newlyPlacedBallsPositions;
        bool allUpcomingPlacedSuccessfully = true;
        for (const QString& color : m_upcomingBallColors) {
            QPoint placedPos = m_grid.placeRandomBall(color);
            if (placedPos.x() < 0 || placedPos.y() < 0) {
                allUpcomingPlacedSuccessfully = false;
                // Game might end here if a ball cannot be placed.
                // This specific ball (and subsequent ones this turn) won't be placed.
                break;
            }
            newlyPlacedBallsPositions.append(placedPos);
        }

        // Check if these newly placed balls formed lines
        if (!newlyPlacedBallsPositions.isEmpty()) {
            QSet<QPoint> linesFromNewBallsSet;
            for (const QPoint& p : newlyPlacedBallsPositions) {
                QList<QPoint> line = m_solver.checkForLines(p.x(), p.y());
                for(const QPoint& lp : line) linesFromNewBallsSet.insert(lp);
            }

            if (!linesFromNewBallsSet.isEmpty()) {
                for (const QPoint& pos : linesFromNewBallsSet) {
                    Ball* b = m_grid.removeBall(pos.x(), pos.y());
                    if (b) {
                        delete b;
                        m_score++; // Simple score for automatic line clears
                    }
                }
                m_scoreLabel->setText(QString("Score: %1").arg(m_score));
            }
        }

        // If not all upcoming balls could be placed, the game might be over.
        // This is checked after drawGrid and generating next upcoming balls.
        if (!allUpcomingPlacedSuccessfully) {
             // Handled by checkGameOver below
        }
    }

    // 3. Always generate and display the *next* set of upcoming balls
    generateUpcomingBalls();
    displayUpcomingBalls();

    // 4. Redraw the grid and clear states
    m_targetMovePos = QPoint(-1, -1); // Reset target move position
    drawGrid();
    m_selectedBallItem = nullptr;

    // 5. Check for game over
    if (checkGameOver()) {
        // Ensure no animation is running if game over is declared from a non-move scenario
        // (e.g. grid fills up after adding upcoming balls without player move)
        if (m_ballAnimation->state() == QAbstractAnimation::Stopped) {
             QMessageBox::information(this, "Game Over", QString("The grid is full! Final Score: %1").arg(m_score));
            if (m_scene) {
                m_scene->removeEventFilter(this); // Disable further interaction
            }
            // Consider disabling other UI elements or offering a "New Game" option
        } else {
            // If animation is running, game over will be re-checked when it stops,
            // or handled because no valid moves can be made.
        }
    }
}

// Implement generateUpcomingBalls, displayUpcomingBalls, checkGameOver

void MainWindow::generateUpcomingBalls() {
    m_upcomingBallColors.clear();
    for (int i = 0; i < 3; ++i) {
        m_upcomingBallColors.append(m_grid.getRandomColor());
    }
}

void MainWindow::displayUpcomingBalls() {
    // m_upcomingBallLabels is an array of 3 QLabel*
    for (int i = 0; i < 3; ++i) {
        if (i < m_upcomingBallColors.size() && m_upcomingBallLabels[i]) {
            const QString& color = m_upcomingBallColors.at(i);
            if (m_ballPixmaps.contains(color)) {
                m_upcomingBallLabels[i]->setPixmap(m_ballPixmaps.value(color));
            } else {
                m_upcomingBallLabels[i]->setText("?"); // Fallback if pixmap missing
                qWarning() << "Missing pixmap for upcoming ball color:" << color;
            }
        } else if (m_upcomingBallLabels[i]) {
            m_upcomingBallLabels[i]->clear(); // Clear if no color (should not happen)
        }
    }
}

bool MainWindow::checkGameOver() {
    // Game is over if there are no empty cells for new balls to be placed.
    // This check is typically done *after* attempting to place upcoming balls.
    // If placeRandomBall fails because grid is full, that's a game over.
    // A simpler check here is just if the grid has any empty cells left.
    // The logic in onAnimationFinished for placing upcoming balls already checks
    // if placeRandomBall fails. If allUpcomingPlacedSuccessfully is false after that loop,
    // and no lines were cleared to make space, it implies game over.
    return m_grid.getEmptyCells().isEmpty();
}
