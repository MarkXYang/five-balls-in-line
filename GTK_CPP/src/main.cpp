#include "MainWindow.h"
#include <gtkmm/application.h>

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create("org.gtkmm.example.color_lines");

    // Use make_window_and_run to let the application create and manage the window.
    return app->make_window_and_run<MainWindow>(argc, argv);
}
