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
#include "../Worker.h"

class MainWindow : public Gtk::Window
{
  private:
    Logic& mLogic;
    std::string mOldTextInEntry{""};

    bool mIgnoreClipboardChange{true};
    std::string mOldClipboard{""};


    //guarded by mutex
    std::string mWaitingToTranslate;
    std::mutex mSearchMutex;
    bool mSearchInProgress{false};
    bool mNewTranslationAvailable{false};
    workerResult mTranslationResult;


  public:
    MainWindow(Logic& logic);
    ~MainWindow();

    // Signal handlers:
    void on_button_clicked();
    void on_clipboard_received(const Gtk::SelectionData &data);


    void executeSearch(std::string text);
    void searchThread();

    //clock function
    bool pulse(int num);

    sigc::connection mPulseConnection;

    // Member widgets:
    Gtk::Grid mGrid;
    Gtk::Entry mWordInput;
    Gtk::Button mAddWordButton;
    Gtk::Button mSettingsButton;
};
