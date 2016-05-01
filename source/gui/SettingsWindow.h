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

#include "Logic.h"

class ModelColumns_dicts : public Gtk::TreeModelColumnRecord
{
  public:
    ModelColumns_dicts()
    {
        add(mEnabled);
        add(mPath);
        add(mPriority);
    }

    Gtk::TreeModelColumn<bool> mEnabled;
    Gtk::TreeModelColumn<Glib::ustring> mPath;
    Gtk::TreeModelColumn<bool> mPriority;
};



class SettingsWindow : public Gtk::Window
{
private:
    Logic& mLogic;

    Gtk::ScrolledWindow mScrollView;
    ModelColumns_dicts mDictViewModel;
    Gtk::TreeView mTreeView;
    Glib::RefPtr<Gtk::ListStore> mRefListStore;

public:
    SettingsWindow(Logic& logic);
    ~SettingsWindow();

};