#include <QApplication>
#include "mainwindow.h" // Include the main window header

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w; // Create an instance of MainWindow
    w.show();
    return a.exec();
}
