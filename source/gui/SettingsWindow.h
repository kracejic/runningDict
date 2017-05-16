#pragma once
#include <future>
#include <mutex>
#include <string>
#include <thread>

#include <gtkmm.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/grid.h>
#include <gtkmm/window.h>

#include "Logic.h"
#include "NewDictWindow.h"

class ModelColumns_dicts : public Gtk::TreeModelColumnRecord
{
  public:
    ModelColumns_dicts()
    {
        add(mEnabled);
        add(mPath);
        add(mBonus);
        add(mStatus);
        add(mError);
        add(mTooltip);
    }

    Gtk::TreeModelColumn<bool> mEnabled;
    Gtk::TreeModelColumn<Glib::ustring> mPath;
    Gtk::TreeModelColumn<bool> mBonus;
    Gtk::TreeModelColumn<Glib::ustring> mStatus;
    Gtk::TreeModelColumn<bool> mError;
    Gtk::TreeModelColumn<Glib::ustring> mTooltip;
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
    Gtk::Label mVersionLabel;
    Gtk::Label mWebSiteLabel;
    Gtk::Button mAddDictButton;

    std::unique_ptr<NewDictWindow> mAddDictWindow;

    Gtk::Box mServerSettingBox;
    Gtk::Label mServerStatus;
    Gtk::CheckButton mToogleServer;
    Gtk::Entry mServer;

    /**
     * handles ESC key
     */
    bool on_key_press_event(GdkEventKey* key_event) override;

    void refreshDicts();
    bool pulse(int num);
    sigc::connection mPulseConnection;
    std::future<void> mServerConnection;

  public:
    SettingsWindow(Logic& logic);
    ~SettingsWindow() override;
};
