#pragma once
#include <string>
#include <thread>
#include <mutex>

#include <gtkmm.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/grid.h>
#include <gtkmm/entry.h>
#include <gtkmm/checkbutton.h>

#include "Logic.h"

class ModelColumns_dicts : public Gtk::TreeModelColumnRecord
{
  public:
    ModelColumns_dicts()
    {
        add(mEnabled);
        add(mPath);
        add(mBonus);
    }

    Gtk::TreeModelColumn<bool> mEnabled;
    Gtk::TreeModelColumn<Glib::ustring> mPath;
    Gtk::TreeModelColumn<bool> mBonus;
};



class SettingsWindow : public Gtk::Window
{
private:
    Logic& mLogic;

    Gtk::ScrolledWindow mScrollView;
    ModelColumns_dicts mDictViewModel;
    Gtk::TreeView mTreeView;
    Glib::RefPtr<Gtk::ListStore> mRefListStore;
    Gtk::Grid mGrid;
    Gtk::CheckButton mToogleFirstCatch;
    Gtk::CheckButton mToogleAlwaysOnTop;

public:
    SettingsWindow(Logic& logic);
    ~SettingsWindow() override;

};