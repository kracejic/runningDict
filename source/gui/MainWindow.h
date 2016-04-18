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

class MainWindow : public Gtk::Window
{
  private:
    Logic& mLogic;
    std::string mOldTextInEntry{""};

    bool mIgnoreClipboardChange{true};
    std::string mOldClipboard{""};


    std::string mWaitingToTranslate;
    std::mutex mWaitingToTranslateMutex;
    std::string mBeingTranslated;
    std::mutex mBeingTranslatedMutex;

  public:
    MainWindow(Logic& logic);

    // Signal handlers:
    void on_button_clicked();
    void on_clipboard_received(const Gtk::SelectionData &data);


    void executeSearch(std::string text);

    //clock function
    bool pulse(int num);

    sigc::connection mPulseConnection;

    // Member widgets:
    Gtk::Grid mGrid;
    Gtk::Entry mWordInput;
    Gtk::Button mAddWordButton;
    Gtk::Button mSettingsButton;
};
