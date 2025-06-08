#include "MainWindow.h"
#include <gtkmm/label.h>
#include <gtkmm/gestureclick.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h> // Required for Gtk::Button
#include <iostream>
#include <string>
#include <vector>
#include <sigc++/sigc++.h> // For sigc::mem_fun

MainWindow::MainWindow()
  : m_gameGrid(9, 9),
    m_pathfinder(&m_gameGrid),
    m_solver(&m_gameGrid),
    m_ballSelected(false),
    m_selectedRow(-1),
    m_selectedCol(-1),
    m_score(0),
    m_gameOver(false),
    m_newGameButton("New Game") { // Initialize New Game button
    set_title("Color Lines GTK");
    set_default_size(450, 600); // Adjusted size for button, score, game over

    // Connect New Game button signal
    m_newGameButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onNewGameClicked));
    m_newGameButton.set_halign(Gtk::Align::CENTER);
    m_newGameButton.set_margin_bottom(10);

    m_scoreLabel.set_text("Score: 0");
    m_scoreLabel.set_halign(Gtk::Align::CENTER);

    m_gameOverLabel.set_text("");
    m_gameOverLabel.set_halign(Gtk::Align::CENTER);

    auto mainBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 5); // Reduced spacing
    mainBox->set_margin(10);

    mainBox->append(m_newGameButton); // Add New Game button at the top
    mainBox->append(m_scoreLabel);
    // m_scoreLabel.set_margin_bottom(5); // Already small spacing from mainBox

    m_grid.set_expand(true);
    m_grid.set_halign(Gtk::Align::CENTER);
    m_grid.set_valign(Gtk::Align::CENTER);
    mainBox->append(m_grid);

    mainBox->append(m_gameOverLabel);
    // m_gameOverLabel.set_margin_top(10); // Spacing from mainBox

    set_child(*mainBox);

    // Initial game setup
    onNewGameClicked(); // Call onNewGameClicked to setup the initial state (board, score, etc.)
                        // This also calls drawBallsOnGrid and checkLinesAndScore.
}

void MainWindow::onNewGameClicked() {
    std::cout << "New Game button clicked." << std::endl;
    m_gameGrid.reset();
    m_score = 0;
    m_scoreLabel.set_text("Score: 0");
    m_gameOver = false;
    m_ballSelected = false;
    m_selectedRow = -1;
    m_selectedCol = -1;
    m_gameOverLabel.set_text("");

    m_gameGrid.addRandomBalls(5); // Add initial balls for the new game
    drawBallsOnGrid();
    checkLinesAndScore(false); // Check if these initial balls formed any lines
}


void MainWindow::drawBallsOnGrid() {
    while (Gtk::Widget* child = m_grid.get_first_child()) {
        m_grid.remove(*child);
    }

    for (int r = 0; r < m_gameGrid.getHeight(); ++r) {
        for (int c = 0; c < m_gameGrid.getWidth(); ++c) {
            const Ball& ball = m_gameGrid.getBall(r, c);
            auto label = Gtk::make_managed<Gtk::Label>(" ");

            switch (ball.getColor()) {
                case BallColor::RED:    label->set_text("R"); break;
                case BallColor::GREEN:  label->set_text("G"); break;
                case BallColor::BLUE:   label->set_text("B"); break;
                case BallColor::YELLOW: label->set_text("Y"); break;
                case BallColor::PURPLE: label->set_text("P"); break;
                case BallColor::EMPTY:  label->set_text(" "); break;
                default:                label->set_text("?"); break;
            }

            label->set_expand(true);
            label->set_halign(Gtk::Align::FILL);
            label->set_valign(Gtk::Align::FILL);
            label->set_vexpand(true);
            label->set_hexpand(true);
            label->set_size_request(30, 30);

            auto gesture = Gtk::GestureClick::create();
            gesture->signal_pressed().connect(
                [this, r, c](int n_press, double x, double y) {
                    this->onCellClicked(r, c);
                }
            );
            label->add_controller(gesture);
            m_grid.attach(*label, c, r);
        }
    }
    m_grid.set_row_spacing(5);
    m_grid.set_column_spacing(5);
}

void MainWindow::onCellClicked(int r, int c) {
    if (m_gameOver) {
        std::cout << "Game is over. No more moves. Click 'New Game'." << std::endl;
        return;
    }

    std::cout << "Cell clicked: (" << r << ", " << c << ")" << std::endl;

    if (m_ballSelected) {
        if (m_gameGrid.isCellEmpty(r, c)) {
            std::cout << "Attempting to move from (" << m_selectedRow << ", " << m_selectedCol << ") to (" << r << ", " << c << ")" << std::endl;
            if (m_pathfinder.canReach(m_selectedRow, m_selectedCol, r, c)) {
                std::cout << "Path found!" << std::endl;
                BallColor color = m_gameGrid.getBall(m_selectedRow, m_selectedCol).getColor();
                m_gameGrid.removeBall(m_selectedRow, m_selectedCol);
                m_gameGrid.placeBall(r, c, color);

                m_ballSelected = false;
                checkLinesAndScore(true);
            } else {
                std::cout << "Invalid move: No path." << std::endl;
                m_ballSelected = false;
                 // Redraw to ensure any selection highlight is removed (if implemented later)
                // drawBallsOnGrid();
            }
        } else {
            std::cout << "Clicked on another ball at (" << r << ", " << c << ")" << std::endl;
            if (m_selectedRow == r && m_selectedCol == c) {
                 std::cout << "Deselected ball at (" << r << ", " << c << ")" << std::endl;
                 m_ballSelected = false;
            } else {
                m_selectedRow = r;
                m_selectedCol = c;
                m_ballSelected = true;
                std::cout << "Switched selection to new ball at (" << r << ", " << c << ")" << std::endl;
            }
            // Redraw to show new selection highlight (if implemented later)
            // drawBallsOnGrid();
        }
    } else {
        if (!m_gameGrid.isCellEmpty(r, c)) {
            m_selectedRow = r;
            m_selectedCol = c;
            m_ballSelected = true;
            std::cout << "Selected ball at (" << r << ", " << c << ")" << std::endl;
            // Redraw to show selection highlight (if implemented later)
            // drawBallsOnGrid();
        } else {
            std::cout << "Clicked on empty cell (" << r << ", " << c << "), no ball selected." << std::endl;
        }
    }
}

int MainWindow::calculateScore(int ballsInLine) {
    if (ballsInLine < 5) return 0;
    return ballsInLine * 10 + (ballsInLine > 5 ? (ballsInLine - 5) * 10 : 0);
}

void MainWindow::checkLinesAndScore(bool ballsMoved) {
    if(m_gameOver && ballsMoved) { // if game was already over, but this is a check after a successful move (should not happen if onCellClicked checks m_gameOver)
         std::cout << "checkLinesAndScore called while game already over. Aborting." << std::endl;
        return;
    }

    std::vector<std::pair<int, int>> lines = m_solver.findLines();

    if (!lines.empty()) {
        std::cout << "Lines found! Number of balls to remove: " << lines.size() << std::endl;
        for (const auto& pos : lines) {
            m_gameGrid.removeBall(pos.first, pos.second);
        }
        m_score += calculateScore(lines.size());
        m_scoreLabel.set_text("Score: " + std::to_string(m_score));

        drawBallsOnGrid();

        std::cout << "Player cleared lines. Gets another turn." << std::endl;
        if (m_gameGrid.isFull()) { // Unlikely, but possible if clearing lines makes it "full" (e.g. no valid moves left, though isFull just means no empty cells)
             m_gameOver = true;
             m_gameOverLabel.set_text("Game Over! (Grid full after line clear)");
             std::cout << "Game Over! Grid became full after clearing lines." << std::endl;
        }
    } else {
        if (ballsMoved) {
            std::cout << "Move successful, no lines formed. Adding new balls." << std::endl;
            std::vector<std::pair<int, int>> newBalls = m_gameGrid.addRandomBalls(3);
            // It's important to draw the grid *after* new balls are added.
            // And then check if these new balls formed lines.

            if (newBalls.empty() && m_gameGrid.isFull()) {
                 drawBallsOnGrid(); // Show the grid in its final state
                 m_gameOver = true;
                 m_gameOverLabel.set_text("Game Over! (No space for new balls)");
                 std::cout << "Game Over! No space to add new balls and grid is full." << std::endl;
                 return;
            }

            // Redraw to show newly added balls FIRST.
            drawBallsOnGrid();

            std::vector<std::pair<int, int>> newLines = m_solver.findLines();
            if (!newLines.empty()) {
                std::cout << "Newly added balls formed lines! Balls to remove: " << newLines.size() << std::endl;
                for (const auto& pos : newLines) {
                    m_gameGrid.removeBall(pos.first, pos.second);
                }
                // Score for these? Typically not, or it's a smaller bonus. For now, no score.
                // m_score += calculateScore(newLines.size());
                // m_scoreLabel.set_text("Score: " + std::to_string(m_score));

                drawBallsOnGrid();
                std::cout << "Lines formed by new balls cleared." << std::endl;
            }

            if (m_gameGrid.isFull()) { // Check again after potential auto-clear
                 m_gameOver = true;
                 m_gameOverLabel.set_text("Game Over! (Grid full after adding balls)");
                 std::cout << "Game Over! Grid is full after adding new balls and auto-clearing." << std::endl;
            }
        } else { // No lines found, and no balls were moved by player (e.g. initial check from New Game)
            std::cout << "No lines formed from initial ball placement." << std::endl;
            // drawBallsOnGrid(); // Already called by onNewGameClicked before this
            if (m_gameGrid.isFull()) { // Check if grid is full from initial placement
                 m_gameOver = true;
                 m_gameOverLabel.set_text("Game Over! (Grid full on start)");
                 std::cout << "Game Over! Grid is full on initial check." << std::endl;
            }
        }
    }
}
