#include "NewDictWindow.h"
#include <algorithm>
#include <cctype>
#include <string>

using namespace std;

NewDictWindow::NewDictWindow(Logic& logic)
    : mLogic(logic)
    , mCreateButton("Create")
{
    this->add(mGrid);
    this->set_default_size(400, -1);
    this->set_title("Create new dictionary");
    this->set_position(Gtk::WIN_POS_MOUSE);

    this->set_border_width(10);

    mGrid.set_column_spacing(5);
    mGrid.set_row_spacing(5);

    mGrid.attach(mDictName, 0, 1, 1, 1);
    mGrid.attach(mCreateButton, 0, 2, 1, 1);

    mDictName.set_placeholder_text("Put one word here");
    mDictName.set_hexpand();
    mDictName.signal_changed().connect([this]() { this->check_validity(); });

    mCreateButton.set_hexpand();
    mCreateButton.signal_clicked().connect([this]() {
        if (any_of(mLogic.mDicts.begin(), mLogic.mDicts.end(), [this](auto& d) {
                return d.getFilename() == mDictName.get_text();
            }))
            return;
        mLogic.createDict(mDictName.get_text());
        this->hide();
    });

    this->show_all_children();
}
//------------------------------------------------------------------------------
bool NewDictWindow::on_key_press_event(GdkEventKey* key_event)
{
    if (key_event->keyval == GDK_KEY_Escape)
    {
        // close the window, when the 'esc' key is pressed
        hide();
        return true;
    }

    return Gtk::Window::on_key_press_event(key_event);
}
//------------------------------------------------------------------------------
void NewDictWindow::check_validity()
{
    // string error_message{""};
    // Gdk::RGBA col("#D62B0B");
    //
    // // check if wordinput contains spaces
    // std::string t = mWordInput.get_text();
    // if (std::any_of(
    //         t.begin(), t.end(), [](auto& p) { return std::isspace(p); }))
    // {
    //     error_message = "Translated word must be without spaces.";
    //     mWordInput.override_color(col);
    // }
    //
    //
    // // check if translation contains atleast one character
    // std::string t2 = this->mWordInput.get_text();
    // if (std::all_of(
    //         t2.begin(), t2.end(), [](auto& p) { return std::isspace(p); }))
    // {
    //     error_message = "Translated word is missing.";
    //     mTranslationInput.override_color(col);
    // }
    //
    // // check if translation contains atleast one character
    // std::string t3 = mTranslationInput.get_text();
    // if (std::all_of(
    //         t3.begin(), t3.end(), [](auto& p) { return std::isspace(p); }))
    // {
    //     error_message = "Translation is missing.";
    //     mTranslationInput.override_color(col);
    // }
    //
    //
    // if (error_message == "")
    // {
    //     // word is OK
    //     mWordInput.unset_color();
    //     mTranslationInput.unset_color();
    //     mAddButton.unset_background_color();
    //     mAddButton.set_label("Add");
    //     mAddButton.set_sensitive(true);
    // }
    // else
    // {
    //     Gdk::RGBA col2("#E5AFA5");
    //     mAddButton.set_label(error_message);
    //     mAddButton.override_background_color(col2);
    //     mAddButton.set_sensitive(false);
    // }
}
