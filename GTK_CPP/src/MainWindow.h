#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm/applicationwindow.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h> // For Gtk::Button
#include "GameGrid.h"
#include "Pathfinder.h"
#include "Solver.h"

class MainWindow : public Gtk::ApplicationWindow {
public:
    MainWindow();

private:
    void drawBallsOnGrid();
    void onCellClicked(int r, int c);
    void checkLinesAndScore(bool ballsMoved);
    int calculateScore(int ballsInLine);
    void onNewGameClicked(); // Handler for New Game button

protected:
    Gtk::Grid m_grid;      // The game board grid
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
