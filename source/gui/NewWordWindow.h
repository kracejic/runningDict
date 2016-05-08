#pragma once
#include <string>
#include <thread>
#include <mutex>

#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/grid.h>
#include <gtkmm/entry.h>

#include "Logic.h"

class NewWordWindow : public Gtk::Window
{
private:
    Gtk::Grid mGrid;
    Logic& mLogic;

    Gtk::Entry mWordInput;
    Gtk::Entry mTranslationInput;
    Gtk::Button mAddButton;

    bool on_key_press_event(GdkEventKey* event) override;

public:
    NewWordWindow(Logic &logic);
    ~NewWordWindow() override;
};

