#include "MainWindow.h"

#include <cstdlib>
#include <gtkmm/application.h>
#include <iostream>


int main(int argc, char* argv[])
{
    std::cout << "starting the app" << std::endl;
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

    Logic logic{};
    // todo OS specific path
    logic.initWithConfig();
    MainWindow mainWindow{logic};

    // // Shows the window and returns when it is closed.
    return app->run(mainWindow);
}
