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


class ModelColumns_dicts_combobox : public Gtk::TreeModelColumnRecord
{
  public:
    ModelColumns_dicts_combobox()
    {
        add(mPath);
    }
    Gtk::TreeModelColumn<Glib::ustring> mPath;
};


class NewWordWindow : public Gtk::Window
{
  private:
    Gtk::Grid mGrid;
    Logic& mLogic;

    Gtk::Entry mWordInput;
    Gtk::Entry mTranslationInput;
    Gtk::Button mAddButton;
    Gtk::ComboBox mCombobox;
    Gtk::Label mComboLabel;


    /// holds combobox with dictionaries
    ModelColumns_dicts_combobox mDictModel;
    Glib::RefPtr<Gtk::ListStore> mRefListStore;

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
    NewWordWindow(Logic& logic, std::string word);
};
