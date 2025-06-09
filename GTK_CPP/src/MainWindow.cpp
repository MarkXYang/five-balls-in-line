#include "MainWindow.h"
#include <gtkmm/box.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/gesturesingle.h> // For Gtk::GestureClick
#include <gdkmm/rgba.h>
#include <cairomm/context.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm> // For std::min
#define _USE_MATH_DEFINES // For M_PI
#include <cmath>     // For M_PI, std::abs
#include <sigc++/sigc++.h> // For sigc::mem_fun

MainWindow::MainWindow()
  : m_gameGrid(9, 9),
    m_pathfinder(&m_gameGrid), // Pass address of m_gameGrid
    m_solver(&m_gameGrid),   // Pass address of m_gameGrid
    m_ballSelected(false),
    m_selectedRow(-1),
    m_selectedCol(-1),
    m_score(0),
    m_gameOver(false),
    m_newGameButton("New Game") {
    set_title("Color Lines GTK");
    set_default_size(450, 600);

    // Main vertical box
    auto mainBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10); // Increased spacing
    mainBox->set_margin(10);
    set_child(*mainBox);

    // New Game Button
    m_newGameButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onNewGameClicked));
    m_newGameButton.set_halign(Gtk::Align::CENTER);
    mainBox->append(m_newGameButton);

    // Score Label
    m_scoreLabel.set_text("Score: 0");
    m_scoreLabel.set_halign(Gtk::Align::CENTER);
    mainBox->append(m_scoreLabel);

    // DrawingArea setup
    m_drawingArea.set_expand(true);
    m_drawingArea.set_halign(Gtk::Align::CENTER);
    m_drawingArea.set_valign(Gtk::Align::CENTER);
    m_drawingArea.set_size_request(360, 360);
    mainBox->append(m_drawingArea);

    m_drawingArea.set_draw_func(sigc::mem_fun(*this, &MainWindow::on_drawingArea_draw));

    auto gesture = Gtk::GestureClick::create();
    gesture->signal_pressed().connect(
        [this](int n_press, double x, double y) {
            double current_drawing_width = m_drawingArea.get_width();
            double current_drawing_height = m_drawingArea.get_height();

            if (m_gameGrid.getWidth() == 0 || m_gameGrid.getHeight() == 0) return;

            double actual_grid_render_size = std::min(current_drawing_width, current_drawing_height);

            double offset_x_click = (current_drawing_width - actual_grid_render_size) / 2.0;
            double offset_y_click = (current_drawing_height - actual_grid_render_size) / 2.0;

            double adjusted_x = x - offset_x_click;
            double adjusted_y = y - offset_y_click;

            double cell_width_double = actual_grid_render_size / m_gameGrid.getWidth();
            double cell_height_double = actual_grid_render_size / m_gameGrid.getHeight();

            if (cell_width_double <= 0 || cell_height_double <= 0) return;

            int c = static_cast<int>(adjusted_x / cell_width_double);
            int r = static_cast<int>(adjusted_y / cell_height_double);

            if (r >= 0 && r < m_gameGrid.getHeight() && c >= 0 && c < m_gameGrid.getWidth() &&
                adjusted_x >=0 && adjusted_x < actual_grid_render_size && // ensure click is within the centered grid box
                adjusted_y >=0 && adjusted_y < actual_grid_render_size) {
                this->onCellClicked(r, c);
            }
        }
    );
    m_drawingArea.add_controller(gesture);

    // Game Over Label
    m_gameOverLabel.set_text("");
    m_gameOverLabel.set_halign(Gtk::Align::CENTER);
    // Example of making it more prominent:
    // Pango::FontDescription fd("Sans Bold 16");
    // m_gameOverLabel.override_font(fd);
    // Gdk::RGBA red("red");
    // m_gameOverLabel.override_color(red);
    mainBox->append(m_gameOverLabel);

    onNewGameClicked(); // Start a new game
}

void MainWindow::onNewGameClicked() {
    std::cout << "New Game button clicked." << std::endl;
    m_gameGrid.reset(); // Resets grid, clears balls
    m_score = 0;
    m_scoreLabel.set_text("Score: 0");
    m_gameOver = false;
    m_ballSelected = false;
    m_selectedRow = -1;
    m_selectedCol = -1;
    m_gameOverLabel.set_text(""); // Clear game over message

    m_gameGrid.addRandomBalls(5); // Add initial balls for the new game
    drawBallsOnGrid(); // Redraw the grid with new balls

    // Check if the newly added balls immediately form lines or if the game is over
    checkLinesAndScore(false); // 'false' because no player move initiated this
}


void MainWindow::on_drawingArea_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
    const Gdk::RGBA GRID_LINE_COLOR("black");
    const Gdk::RGBA CELL_BG_COLOR("white");
    const Gdk::RGBA SELECTED_CELL_HIGHLIGHT_COLOR("lightgray"); // For selected ball's cell

    std::map<BallColor, Gdk::RGBA> BALL_COLORS;
    BALL_COLORS[BallColor::RED] = Gdk::RGBA("red");
    BALL_COLORS[BallColor::GREEN] = Gdk::RGBA("green");
    BALL_COLORS[BallColor::BLUE] = Gdk::RGBA("blue");
    BALL_COLORS[BallColor::YELLOW] = Gdk::RGBA("yellow");
    BALL_COLORS[BallColor::PURPLE] = Gdk::RGBA("purple");

    int num_rows = m_gameGrid.getHeight();
    int num_cols = m_gameGrid.getWidth();
    if (num_rows == 0 || num_cols == 0) return;

    double actual_grid_render_size = std::min(static_cast<double>(width), static_cast<double>(height));
    double cell_width = actual_grid_render_size / num_cols;
    double cell_height = actual_grid_render_size / num_rows;

    double offset_x = (width - actual_grid_render_size) / 2.0;
    double offset_y = (height - actual_grid_render_size) / 2.0;

    cr->save();
    cr->translate(offset_x, offset_y);

    for (int r = 0; r < num_rows; ++r) {
        for (int c = 0; c < num_cols; ++c) {
            double cell_x_pos = c * cell_width;
            double cell_y_pos = r * cell_height;

            // Highlight selected cell
            if (m_ballSelected && r == m_selectedRow && c == m_selectedCol) {
                Gdk::Cairo::set_source_rgba(cr, SELECTED_CELL_HIGHLIGHT_COLOR);
            } else {
                Gdk::Cairo::set_source_rgba(cr, CELL_BG_COLOR);
            }
            cr->rectangle(cell_x_pos, cell_y_pos, cell_width, cell_height);
            cr->fill_preserve();

            Gdk::Cairo::set_source_rgba(cr, GRID_LINE_COLOR);
            cr->set_line_width(1.0);
            cr->stroke();

            const Ball& ball = m_gameGrid.getBall(r, c);
            if (ball.getColor() != BallColor::EMPTY) {
                auto it = BALL_COLORS.find(ball.getColor());
                if (it != BALL_COLORS.end()) {
                    Gdk::Cairo::set_source_rgba(cr, it->second);
                    double ball_radius = std::min(cell_width, cell_height) / 2.8;
                    cr->arc(cell_x_pos + cell_width / 2.0, cell_y_pos + cell_height / 2.0, ball_radius, 0.0, 2.0 * M_PI);
                    cr->fill();
                }
            }
        }
    }
    cr->restore();
}

void MainWindow::drawBallsOnGrid() {
    if (m_drawingArea.get_widget() && m_drawingArea.get_realized()) { // Check if widget is realized
        m_drawingArea.queue_draw();
    }
}

void MainWindow::onCellClicked(int r, int c) {
    if (m_gameOver) {
        std::cout << "Game is over. No more moves. Click 'New Game'." << std::endl;
        return; // Don't allow moves if game is over
    }

    std::cout << "Cell clicked: (" << r << ", " << c << ")" << std::endl;

    if (m_ballSelected) {
        // A ball is already selected, try to move it
        if (m_gameGrid.isCellEmpty(r, c)) {
            // Clicked on an empty cell, attempt to move
            std::cout << "Attempting to move from (" << m_selectedRow << ", " << m_selectedCol << ") to (" << r << ", " << c << ")" << std::endl;
            if (m_pathfinder.canReach(m_selectedRow, m_selectedCol, r, c)) {
                std::cout << "Path found!" << std::endl;
                BallColor color = m_gameGrid.getBall(m_selectedRow, m_selectedCol).getColor();
                m_gameGrid.removeBall(m_selectedRow, m_selectedCol);
                m_gameGrid.placeBall(r, c, color);

                m_ballSelected = false; // Deselect after moving
                checkLinesAndScore(true); // Balls moved, check for lines, add new balls if no lines
            } else {
                std::cout << "Invalid move: No path." << std::endl;
                // Keep ball selected or deselect? Game rules vary. Let's deselect for simplicity.
                m_ballSelected = false;
            }
        } else {
            // Clicked on another ball
             std::cout << "Clicked on another ball at (" << r << ", " << c << ")" << std::endl;
            if (m_selectedRow == r && m_selectedCol == c) {
                 // Clicked on the already selected ball - deselect it
                 std::cout << "Deselected ball at (" << r << ", " << c << ")" << std::endl;
                 m_ballSelected = false;
            } else {
                // Selected a different ball
                m_selectedRow = r;
                m_selectedCol = c;
                // m_ballSelected remains true
                std::cout << "Switched selection to new ball at (" << r << ", " << c << ")" << std::endl;
            }
        }
    } else {
        // No ball selected yet, try to select this one
        if (!m_gameGrid.isCellEmpty(r, c)) {
            m_selectedRow = r;
            m_selectedCol = c;
            m_ballSelected = true;
            std::cout << "Selected ball at (" << r << ", " << c << ")" << std::endl;
        } else {
            std::cout << "Clicked on empty cell (" << r << ", " << c << "), no ball selected." << std::endl;
        }
    }
    drawBallsOnGrid(); // Redraw to reflect selection changes or moves
}

int MainWindow::calculateScore(int ballsInLine) {
    if (ballsInLine < 5) return 0;
    // Example: 5 balls = 10, 6 = 10+12=22, 7 = 22+14=36 etc. (base 10, +2 for each extra ball)
    // A simpler scoring: 10 points for 5 balls, 5 more for each additional ball
    return 10 + (ballsInLine - 5) * 5;
}

void MainWindow::checkLinesAndScore(bool ballsMovedAndNoLinesFormedByPlayer) {
    if (m_gameOver) return; // Don't process if already game over

    std::vector<std::pair<int, int>> lines = m_solver.findLines();

    if (!lines.empty()) {
        std::cout << "Lines found! Number of balls to remove: " << lines.size() << std::endl;
        for (const auto& pos : lines) {
            m_gameGrid.removeBall(pos.first, pos.second);
        }
        m_score += calculateScore(lines.size());
        m_scoreLabel.set_text("Score: " + std::to_string(m_score));

        // Player cleared lines, does not add new balls this turn.
        // The board might be full now, check for game over.
        if (m_gameGrid.isFull()) {
             m_gameOver = true;
             m_gameOverLabel.set_markup("<span size='large' weight='bold' foreground='red'>Game Over! Grid Full!</span>");
             std::cout << "Game Over! Grid became full after clearing lines." << std::endl;
        }
    } else {
        // No lines were formed by the player's move OR this is an initial check.
        if (ballsMovedAndNoLinesFormedByPlayer) {
            // Player moved a ball, but it didn't form a line. Add new balls.
            std::cout << "Move successful, no lines formed by player. Adding new balls." << std::endl;
            std::vector<std::pair<int, int>> newBallsPositions = m_gameGrid.addRandomBalls(3);

            if (newBallsPositions.empty() && m_gameGrid.isFull()) {
                 // No space for new balls and grid is full. Game Over.
                 m_gameOver = true;
                 m_gameOverLabel.set_markup("<span size='large' weight='bold' foreground='red'>Game Over! No Space!</span>");
                 std::cout << "Game Over! No space to add new balls and grid is full." << std::endl;
            } else {
                // New balls were added (or there was space). Check if THEY formed lines.
                std::vector<std::pair<int, int>> newLinesFromAddedBalls = m_solver.findLines();
                if (!newLinesFromAddedBalls.empty()) {
                    std::cout << "Newly added balls formed lines! Balls to remove: " << newLinesFromAddedBalls.size() << std::endl;
                    for (const auto& pos : newLinesFromAddedBalls) {
                        m_gameGrid.removeBall(pos.first, pos.second);
                    }
                    // Score for these? Some games do, some don't. Let's not add to score for this.
                    // m_score += calculateScore(newLinesFromAddedBalls.size());
                    // m_scoreLabel.set_text("Score: " + std::to_string(m_score));
                    std::cout << "Lines formed by new balls cleared." << std::endl;

                    if (m_gameGrid.isFull()) { // Check again after auto-clear
                         m_gameOver = true;
                         m_gameOverLabel.set_markup("<span size='large' weight='bold' foreground='red'>Game Over! Grid Full!</span>");
                         std::cout << "Game Over! Grid is full after adding balls and auto-clearing." << std::endl;
                    }
                }
            }
        } else if (!ballsMovedAndNoLinesFormedByPlayer && m_gameGrid.isFull()) {
            // This case is for initial setup (onNewGameClicked calls with false)
            // If grid is full immediately after adding first balls and no lines formed.
            m_gameOver = true;
            m_gameOverLabel.set_markup("<span size='large' weight='bold' foreground='red'>Game Over! Grid Full on Start!</span>");
            std::cout << "Game Over! Grid is full on initial ball placement and no lines." << std::endl;
        }
    }
    // Always redraw at the end of a turn or check.
    drawBallsOnGrid();
}
