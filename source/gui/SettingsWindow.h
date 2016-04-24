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

class SettingsWindow : public Gtk::TreeModelColumnRecord
{
public:
    SettingsWindow();
    ~SettingsWindow();

};