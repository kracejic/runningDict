#pragma once
#include <mutex>
#include <string>
#include <thread>

#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/entry.h>
#include <gtkmm/grid.h>
#include <gtkmm/window.h>

#include "Logic.h"


class ChangeWordWindow : public Gtk::Window
{
  private:
    Gtk::Grid mGrid;
    Logic& mLogic;

    Gtk::Entry mWordInput;
    Gtk::Entry mTranslationInput;
    Gtk::Label mDictLabel;
    Gtk::Button mAddButton;
    Gtk::Button mDeleteButton;

    std::string mDictFilename;
    std::string mChangedWord;


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
    ChangeWordWindow(Logic& logic, std::string word, std::string translation,
        std::string dictFilename);
};
