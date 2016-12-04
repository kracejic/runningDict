#include "ChangeWordWindow.h"
#include <algorithm>
#include <cctype>
#include <string>

using namespace std;

ChangeWordWindow::ChangeWordWindow(Logic& logic, std::string word,
    std::string translation, std::string dictFilename)
    : mLogic(logic)
    , mAddButton("Add")
{
    this->add(mGrid);
    this->set_default_size(400, -1);
    this->set_title("Change a word \'" + word + "\'");
    this->set_position(Gtk::WIN_POS_MOUSE);
    mDictFilename = dictFilename;
    mChangedWord = word;
    mDictLabel.set_text(dictFilename);

    set_border_width(10);

    mGrid.set_column_spacing(5);
    mGrid.set_row_spacing(5);


    mGrid.attach(mWordInput, 0, 1, 2, 1);
    mGrid.attach(mTranslationInput, 0, 2, 2, 1);
    mGrid.attach(mDictLabel, 0, 3, 2, 1);
    mGrid.attach(mDeleteButton, 0, 4, 1, 1);
    mGrid.attach(mAddButton, 1, 4, 1, 1);

    Gdk::RGBA col2("#E5AFA5");
    mDeleteButton.set_label("Delete word");
    mDeleteButton.override_background_color(col2);

    mWordInput.set_placeholder_text("Put one word here");
    mWordInput.set_text(word);
    mWordInput.set_hexpand();
    mWordInput.signal_changed().connect([this]() { this->check_validity(); });

    // TODO inform further about missing translation text
    // maybe pull out logic for setting backround on button into member function


    mTranslationInput.set_placeholder_text("translation goes here");
    mTranslationInput.set_text(translation);
    mTranslationInput.set_hexpand();
    mTranslationInput.grab_focus();
    mTranslationInput.signal_changed().connect(
        [this]() { this->check_validity(); });


    mAddButton.signal_clicked().connect([this]() {
        auto dict = find_if(mLogic.mDicts.begin(), mLogic.mDicts.end(),
            [this](auto& d) { return d.getFilename() == mDictFilename; });
        if (dict != mLogic.mDicts.end())
        {
            dict->changeWord(mChangedWord, mTranslationInput.get_text(),
                mWordInput.get_text());
            this->hide();
        }
        else
            cout << "[error] dict " << mLogic.mLastDictForNewWord
                 << " not found" << endl;
    });

    // create model for combobox with Dict selection


    this->check_validity();
    this->show_all_children();
}
//------------------------------------------------------------------------------
bool ChangeWordWindow::on_key_press_event(GdkEventKey* key_event)
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
void ChangeWordWindow::check_validity()
{
    string error_message{""};
    Gdk::RGBA col("#D62B0B");

    // check if wordinput contains spaces
    std::string t = mWordInput.get_text();
    if (std::any_of(
            t.begin(), t.end(), [](auto& p) { return std::isspace(p); }))
    {
        error_message = "Translated word must be without spaces.";
        mWordInput.override_color(col);
    }


    // check if translation contains atleast one character
    std::string t2 = this->mWordInput.get_text();
    if (std::all_of(
            t2.begin(), t2.end(), [](auto& p) { return std::isspace(p); }))
    {
        error_message = "Translated word is missing.";
        mTranslationInput.override_color(col);
    }

    // check if translation contains atleast one character
    std::string t3 = mTranslationInput.get_text();
    if (std::all_of(
            t3.begin(), t3.end(), [](auto& p) { return std::isspace(p); }))
    {
        error_message = "Translation is missing.";
        mTranslationInput.override_color(col);
    }


    if (error_message == "")
    {
        // word is OK
        mWordInput.unset_color();
        mTranslationInput.unset_color();
        mAddButton.unset_background_color();
        mAddButton.set_label("Change");
        mAddButton.set_sensitive(true);
    }
    else
    {
        Gdk::RGBA col2("#E5AFA5");
        mAddButton.set_label(error_message);
        mAddButton.override_background_color(col2);
        mAddButton.set_sensitive(false);
    }
}
