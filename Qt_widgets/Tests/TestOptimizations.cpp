#include <QtTest/QtTest>
#include "../Pathfinder.h"
#include "../Solver.h"
#include "../Grid.h"
#include "../Ball.h" // For creating Ball instances in tests
#include <QSet>      // For comparing lists of points where order doesn't matter
#include <QPoint>    // For QPoint
#include <QStringList> // For QStringList
#include <QDebug>    // For QDebug (optional)

// Helper to convert QList<QPoint> to QSet<QPoint> for comparison
// Also useful for QHash<QPoint, T> if QPoint needs custom hash.
// For QSet<QPoint>, Qt usually provides this if <QPoint> and <QSet> are included.
// However, explicit qtpoint_hash.h might be needed in some projects.
// For this test, assuming QSet<QPoint> works directly.
QSet<QPoint> toPointSet(const QList<QPoint>& list) {
    QSet<QPoint> set;
    for (const QPoint& p : list) {
        set.insert(p);
    }
    return set;
}

class TestOptimizations : public QObject
{
    Q_OBJECT

private slots:
    // Pathfinder Tests
    void testPathfinder_data();
    void testPathfinder();

    // Solver Tests
    void testSolver_data();
    void testSolver();

    // Grid Tests
    void testGrid_initialization();
    void testGrid_placeAndRemoveBall();
    void testGrid_randomBallPlacement();
    void testGrid_fullGrid();
    void testGrid_ballCount();

public:
    TestOptimizations() {} // Constructor

};

// --- Pathfinder Tests ---
void TestOptimizations::testPathfinder_data()
{
    QTest::addColumn<QPoint>("start");
    QTest::addColumn<QPoint>("end");
    QTest::addColumn<QList<QPoint>>("expectedPath");
    QTest::addColumn<QStringList>("gridSetup"); // List of "x,y,colorOrObstacle"

    // Scenario 1: Simple path
    QTest::newRow("simple path")
        << QPoint(0,0) << QPoint(0,2)
        << (QList<QPoint>() << QPoint(0,0) << QPoint(0,1) << QPoint(0,2))
        << QStringList();

    // Scenario 2: No path (blocked)
    QTest::newRow("no path - blocked")
        << QPoint(0,0) << QPoint(0,2)
        << QList<QPoint>() // Expect empty path
        << (QStringList() << "0,1,obstacle"); // Obstacle at (0,1)

    // Scenario 3: Start == End (empty cell)
    QTest::newRow("start is end - empty")
        << QPoint(0,0) << QPoint(0,0)
        << QList<QPoint>() // Expect empty path (no move needed by current Pathfinder rules)
        << QStringList();

    // Scenario 4: Start == End (occupied cell)
    // Pathfinder's findPath checks if end is empty. If start==end and end is occupied, it's like end is occupied.
    QTest::newRow("start is end - occupied")
        << QPoint(0,0) << QPoint(0,0)
        << QList<QPoint>()
        << (QStringList() << "0,0,red");


    // Scenario 5: End point occupied
    QTest::newRow("end occupied")
        << QPoint(0,0) << QPoint(0,1)
        << QList<QPoint>()
        << (QStringList() << "0,1,red");

    // Scenario 6: Start out of bounds
    QTest::newRow("start out of bounds")
        << QPoint(-1,0) << QPoint(0,2)
        << QList<QPoint>()
        << QStringList();

    // Scenario 7: End out of bounds
    QTest::newRow("end out of bounds")
        << QPoint(0,0) << QPoint(Grid::GRID_SIZE, 0)
        << QList<QPoint>()
        << QStringList();

    // Scenario 8: More complex path with obstacles
    // Grid: S . .   (S=(0,0), E=(2,2))
    //       O O .   (O=Obstacle at (0,1), (1,1))
    //       . O E   (O=Obstacle at (1,2)) -> This was error in plan, (1,2) was obstacle
    // Corrected obstacles from plan: "0,1,obstacle" "1,1,obstacle" "1,2,obstacle"
    // Path: (0,0)->(1,0)->(2,0)->(2,1)->(2,2)
    QTest::newRow("complex path")
        << QPoint(0,0) << QPoint(2,2)
        << (QList<QPoint>() << QPoint(0,0) << QPoint(1,0) << QPoint(2,0) << QPoint(2,1) << QPoint(2,2))
        << (QStringList() << "0,1,obstacle" << "1,1,obstacle" << "1,2,obstacle");
}

void TestOptimizations::testPathfinder()
{
    QFETCH(QPoint, start);
    QFETCH(QPoint, end);
    QFETCH(QList<QPoint>, expectedPath);
    QFETCH(QStringList, gridSetup);

    Grid testGrid;
    // Setup grid based on gridSetup
    // Balls placed are owned by the grid. They will be deleted when testGrid is destroyed.
    for (const QString& item : gridSetup) {
        QStringList parts = item.split(',');
        int x = parts[0].toInt();
        int y = parts[1].toInt();
        // For pathfinder tests, any ball makes a cell non-empty (an obstacle).
        // The color or ID doesn't matter for pathfinding logic itself.
        testGrid.placeBall(x, y, new Ball(parts[2], 0)); // ID 0 is arbitrary
    }

    Pathfinder pathfinder(&testGrid);
    QList<QPoint> actualPath = pathfinder.findPath(start, end);

    QCOMPARE(actualPath, expectedPath);
    // testGrid destructor will clean up balls
}


// --- Solver Tests ---
void TestOptimizations::testSolver_data()
{
    QTest::addColumn<QPoint>("movedBallPos");
    QTest::addColumn<QString>("movedBallColor");
    QTest::addColumn<QList<QPoint>>("expectedLinePoints"); // All points part of lines of 5+
    QTest::addColumn<QStringList>("initialBalls"); // "x,y,color"

    // Scenario 1: Horizontal line of 5
    QTest::newRow("horizontal line 5")
        << QPoint(0,4) << "red" // Ball at (0,4) is the last one completing the line
        << (QList<QPoint>() << QPoint(0,0) << QPoint(0,1) << QPoint(0,2) << QPoint(0,3) << QPoint(0,4))
        << (QStringList() << "0,0,red" << "0,1,red" << "0,2,red" << "0,3,red" << "0,4,red");

    // Scenario 2: Vertical line of 5
    QTest::newRow("vertical line 5")
        << QPoint(4,0) << "blue"
        << (QList<QPoint>() << QPoint(0,0) << QPoint(1,0) << QPoint(2,0) << QPoint(3,0) << QPoint(4,0))
        << (QStringList() << "0,0,blue" << "1,0,blue" << "2,0,blue" << "3,0,blue" << "4,0,blue");

    // Scenario 3: Diagonal (TL-BR) line of 5
    QTest::newRow("diag TL-BR line 5")
        << QPoint(4,4) << "green"
        << (QList<QPoint>() << QPoint(0,0) << QPoint(1,1) << QPoint(2,2) << QPoint(3,3) << QPoint(4,4))
        << (QStringList() << "0,0,green" << "1,1,green" << "2,2,green" << "3,3,green" << "4,4,green");

    // Scenario 4: Diagonal (BL-TR) line of 5
    QTest::newRow("diag BL-TR line 5")
        << QPoint(0,4) << "yellow" // Ball at (0,4) completes line from (4,0)
        << (QList<QPoint>() << QPoint(4,0) << QPoint(3,1) << QPoint(2,2) << QPoint(1,3) << QPoint(0,4))
        << (QStringList() << "4,0,yellow" << "3,1,yellow" << "2,2,yellow" << "1,3,yellow" << "0,4,yellow");

    // Scenario 5: Line longer than 5
    QTest::newRow("horizontal line >5")
        << QPoint(0,3) << "red" // Moved ball is part of the line
        << (QList<QPoint>() << QPoint(0,0) << QPoint(0,1) << QPoint(0,2) << QPoint(0,3) << QPoint(0,4) << QPoint(0,5))
        << (QStringList() << "0,0,red" << "0,1,red" << "0,2,red" << "0,3,red" << "0,4,red" << "0,5,red");

    // Scenario 6: No line of 5
    QTest::newRow("no line")
        << QPoint(0,0) << "red"
        << QList<QPoint>() // Empty list expected
        << (QStringList() << "0,0,red" << "0,1,red" << "1,0,blue");

    // Scenario 7: Multiple lines (L-shape), all points from both lines should be included
    QList<QPoint> lShapePoints;
    lShapePoints << QPoint(2,0) << QPoint(2,1) << QPoint(2,2) << QPoint(2,3) << QPoint(2,4); // Vertical
    lShapePoints << QPoint(0,2) << QPoint(1,2) /* << QPoint(2,2) included */ << QPoint(3,2) << QPoint(4,2); // Horizontal
    QTest::newRow("multiple lines L-shape")
        << QPoint(2,2) << "purple" // Center of L
        << lShapePoints
        << (QStringList()
            // Vertical part
            << "2,0,purple" << "2,1,purple" << "2,2,purple" << "2,3,purple" << "2,4,purple"
            // Horizontal part
            << "0,2,purple" << "1,2,purple" /* << "2,2,purple" (already listed for vertical) */ << "3,2,purple" << "4,2,purple"
           );
}

void TestOptimizations::testSolver()
{
    QFETCH(QPoint, movedBallPos);
    QFETCH(QString, movedBallColor);
    QFETCH(QList<QPoint>, expectedLinePoints);
    QFETCH(QStringList, initialBalls);

    Grid testGrid;
    int ballId = 1;
    for (const QString& item : initialBalls) {
        QStringList parts = item.split(',');
        testGrid.placeBall(parts[0].toInt(), parts[1].toInt(), new Ball(parts[2], ballId++));
    }

    Ball* ballAtMovedPos = testGrid.getBallAt(movedBallPos.x(), movedBallPos.y());
    QVERIFY2(ballAtMovedPos != nullptr, "Test data problem: movedBallPos should have a ball.");
    QVERIFY2(ballAtMovedPos->getColor() == movedBallColor, "Test data problem: movedBallColor doesn't match ball at movedBallPos.");

    Solver solver(&testGrid);
    QList<QPoint> actualLinePoints = solver.checkForLines(movedBallPos.x(), movedBallPos.y());

    QCOMPARE(toPointSet(actualLinePoints), toPointSet(expectedLinePoints));
    // testGrid destructor will clean up balls
}


// --- Grid Tests ---
void TestOptimizations::testGrid_initialization()
{
    Grid testGrid;
    QCOMPARE(testGrid.getBallCount(), 0);
    QCOMPARE(testGrid.getEmptyCells().size(), Grid::GRID_SIZE * Grid::GRID_SIZE);

    QList<QPoint> allCellsList;
    for (int i = 0; i < Grid::GRID_SIZE; ++i) {
        for (int j = 0; j < Grid::GRID_SIZE; ++j) {
            allCellsList.append(QPoint(i,j));
            QVERIFY(testGrid.isCellEmpty(i,j));
        }
    }
    QCOMPARE(toPointSet(testGrid.getEmptyCells()), toPointSet(allCellsList));
}

void TestOptimizations::testGrid_placeAndRemoveBall()
{
    Grid testGrid;
    Ball* ball1 = new Ball("red", 1); // Grid will take ownership if placeBall is successful
    QPoint p1(0,0);

    QVERIFY(testGrid.placeBall(p1.x(), p1.y(), ball1));
    QCOMPARE(testGrid.getBallCount(), 1);
    QCOMPARE(testGrid.getEmptyCells().size(), Grid::GRID_SIZE * Grid::GRID_SIZE - 1);
    QVERIFY(!testGrid.isCellEmpty(p1.x(), p1.y()));
    QVERIFY(!testGrid.getEmptyCells().contains(p1));
    QCOMPARE(testGrid.getBallAt(p1.x(), p1.y()), ball1);

    Ball* ball2 = new Ball("blue", 2);
    QVERIFY(!testGrid.placeBall(p1.x(), p1.y(), ball2)); // Already occupied
    delete ball2; // Clean up ball2 as it wasn't placed and Grid didn't take ownership

    Ball* removedBall = testGrid.removeBall(p1.x(), p1.y()); // Grid gives up ownership
    QCOMPARE(removedBall, ball1);
    delete removedBall; // Now we are responsible for deleting the removed ball

    QCOMPARE(testGrid.getBallCount(), 0);
    QCOMPARE(testGrid.getEmptyCells().size(), Grid::GRID_SIZE * Grid::GRID_SIZE);
    QVERIFY(testGrid.isCellEmpty(p1.x(), p1.y()));
    QVERIFY(testGrid.getEmptyCells().contains(p1));
}

void TestOptimizations::testGrid_randomBallPlacement()
{
    Grid testGrid;
    int initialEmptySize = testGrid.getEmptyCells().size();

    QPoint placedPos = testGrid.placeRandomBall("green");
    QVERIFY(placedPos.x() != -1 && placedPos.y() != -1);
    QCOMPARE(testGrid.getBallCount(), 1);
    QCOMPARE(testGrid.getEmptyCells().size(), initialEmptySize - 1);
    QVERIFY(!testGrid.getEmptyCells().contains(placedPos));
    Ball* placedBall = testGrid.getBallAt(placedPos.x(), placedPos.y());
    QVERIFY(placedBall != nullptr);
    if(placedBall) { // Check to be safe before dereferencing
        QCOMPARE(placedBall->getColor(), QString("green"));
    }
    // Ball placed by placeRandomBall is owned by grid, will be cleaned by testGrid destructor
}

void TestOptimizations::testGrid_fullGrid()
{
    Grid testGrid;
    int totalCells = Grid::GRID_SIZE * Grid::GRID_SIZE;
    for (int i = 0; i < totalCells; ++i) {
        QString color = QString("color%1").arg(i);
        QPoint p = testGrid.placeRandomBall(color);
        QVERIFY2(p.x() != -1, QString("Failed to place ball %1 on non-full grid. Empty cells: %2")
                                .arg(i+1).arg(testGrid.getEmptyCells().size()).toStdString().c_str());
    }

    QCOMPARE(testGrid.getBallCount(), totalCells);
    QVERIFY(testGrid.getEmptyCells().isEmpty());

    QPoint p_fail = testGrid.placeRandomBall("extracolor");
    QCOMPARE(p_fail, QPoint(-1,-1)); // Should fail, grid is full

    // testGrid destructor will clean up all balls
}

void TestOptimizations::testGrid_ballCount() {
    Grid testGrid;
    QCOMPARE(testGrid.getBallCount(), 0);

    // Ball created here will be owned by grid
    testGrid.placeBall(0,0, new Ball("red",1));
    QCOMPARE(testGrid.getBallCount(), 1);

    // Ball created here will be owned by grid
    testGrid.placeBall(0,1, new Ball("blue",2));
    QCOMPARE(testGrid.getBallCount(), 2);

    Ball* b1 = testGrid.removeBall(0,0); // Grid gives up ownership
    QCOMPARE(testGrid.getBallCount(), 1);
    delete b1; // Caller must delete

    Ball* b2 = testGrid.removeBall(0,1); // Grid gives up ownership
    QCOMPARE(testGrid.getBallCount(), 0);
    delete b2; // Caller must delete

    Ball* b3 = testGrid.removeBall(0,0); // Removing from empty cell
    QVERIFY(b3 == nullptr);
    QCOMPARE(testGrid.getBallCount(), 0);
    // No ball to delete for b3
}

QTEST_MAIN(TestOptimizations)
#include "TestOptimizations.moc" // If using QObject and slots, MOC is needed.
