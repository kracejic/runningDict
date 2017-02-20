#pragma once
#include <mutex>
#include <string>
#include <thread>

#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/grid.h>
#include <gtkmm/window.h>

#include "Logic.h"


class NewDictWindow : public Gtk::Window
{
  private:
    Gtk::Grid mGrid;
    Logic& mLogic;

    Gtk::Entry mDictName;
    Gtk::Button mCreateButton;


    /**
     * handles ESC key
     */
    bool on_key_press_event(GdkEventKey* key_event) override;

    /**
     * Check validity of data and sets add button accordigly.
     * So if there is a space in word, or translation is missing, add button
     * will be inactive and with red error message.
     */
    void check_validity();

  public:
    NewDictWindow(Logic& logic);
};
