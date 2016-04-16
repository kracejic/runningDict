#pragma once
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/grid.h>
#include <gtkmm/entry.h>

#include "Logic.h"

class MainWindow : public Gtk::Window
{
  private:
    Logic& mLogic;

  public:
    MainWindow(Logic& logic);

    // Signal handlers:
    void on_button_clicked();

    // Member widgets:
    Gtk::Grid mGrid;
    Gtk::Entry mWordInput;
    Gtk::Button mAddWordButton;
    Gtk::Button mSettingsButton;
};
