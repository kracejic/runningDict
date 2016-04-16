#include "MainWindow.h"

#include <gtkmm/application.h>
#include <cstdlib>





int main(int argc, char *argv[])
{
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

    Logic logic{argc, argv};
    MainWindow mainWindow{logic};

    // // Shows the window and returns when it is closed.
    return app->run(mainWindow);
}

