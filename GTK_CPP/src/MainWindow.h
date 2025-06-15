#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm/applicationwindow.h>
// #include <gtkmm/grid.h> // Replaced by DrawingArea
#include <gtkmm/drawingarea.h> // Added
#include <gtkmm/label.h>
#include <gtkmm/button.h> // For Gtk::Button
#include "GameGrid.h"
#include "Pathfinder.h"
#include "Solver.h"

class MainWindow : public Gtk::ApplicationWindow {
public:
    MainWindow();

private:
    void drawBallsOnGrid(); // Will now just call m_drawingArea.queue_draw()
    void onCellClicked(int r, int c);
    void checkLinesAndScore(bool ballsMoved);
    int calculateScore(int ballsInLine);
    void onNewGameClicked(); // Handler for New Game button

    // Drawing handler for the game board
    void on_drawingArea_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

protected:
    // Gtk::Grid m_grid;      // Replaced by m_drawingArea
    Gtk::DrawingArea m_drawingArea; // Used for custom drawing the game board
    GameGrid m_gameGrid;   // The logical game grid
    Pathfinder m_pathfinder;
    Solver m_solver;

    bool m_ballSelected;
    int m_selectedRow, m_selectedCol;

    Gtk::Label m_scoreLabel;
    int m_score;

    Gtk::Label m_gameOverLabel;
    bool m_gameOver;
    Gtk::Button m_newGameButton; // New Game button
};

#endif //MAINWINDOW_H
