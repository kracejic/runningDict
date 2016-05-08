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
#include "SettingsWindow.h"
#include "NewWordWindow.h"
#include "../Worker.h"

class ModelColumns : public Gtk::TreeModelColumnRecord
{
  public:
    ModelColumns()
    {
        add(mGerman);
        add(mGerman_found);
        add(mEnglish);
        add(mScore);
    }

    Gtk::TreeModelColumn<Glib::ustring> mGerman;
    Gtk::TreeModelColumn<Glib::ustring> mGerman_found;
    Gtk::TreeModelColumn<Glib::ustring> mEnglish;
    Gtk::TreeModelColumn<int> mScore;
};


class MainWindow : public Gtk::Window
{
  private:
    std::unique_ptr<SettingsWindow> mSettingsWindow;
    std::unique_ptr<NewWordWindow> mNewWordWindow;

    Logic& mLogic;
    Glib::ustring mOldTextInEntry{""};

    bool mIgnoreClipboardChange{true};
    Glib::ustring mOldClipboard{""};

    ModelColumns mColumns;
    Glib::RefPtr<Gtk::ListStore> mRefListStore;
    Gtk::TreeView mTreeView;


    //guarded by mutex
    Glib::ustring mWaitingToTranslate;
    std::mutex mSearchMutex;
    bool mSearchInProgress{false};
    bool mNewTranslationAvailable{false};
    bool mRedrawNeeded{false};
    workerResult mTranslationResult;
    std::vector<std::string> mTranslationWords;

    Gtk::CellRendererText mCellRenderer;

  public:
    MainWindow(Logic& logic);
    ~MainWindow();

    // Signal handlers:
    void on_button_clicked();
    void on_clipboard_received(const Glib::ustring &data);

    void executeSearch(Glib::ustring text);
    void searchThread();

    //clock function
    bool pulse(int num);

    sigc::connection mPulseConnection;

    // Member widgets:
    Gtk::Grid mGrid;
    // Gtk::Grid mResultGrid;
    Gtk::ScrolledWindow mScrollForResults;
    Gtk::Entry mWordInput;
    Gtk::Button mAddWordButton;
    Gtk::Button mSettingsButton;
};
