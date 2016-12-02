#include "NewWordWindow.h"
#include <algorithm>
#include <cctype>
#include <string>

using namespace std;

NewWordWindow::NewWordWindow(Logic& logic, std::string word)
    : mLogic(logic)
    , mAddButton("Add")
    , mComboLabel("Target dictionary: ")
{
    this->add(mGrid);
    this->set_default_size(400, -1);
    this->set_title("Add word");
    this->set_position(Gtk::WIN_POS_MOUSE);

    set_border_width(10);

    mGrid.set_column_spacing(5);
    mGrid.set_row_spacing(5);


    mGrid.attach(mWordInput, 0, 1, 2, 1);
    mGrid.attach(mTranslationInput, 0, 2, 2, 1);
    mGrid.attach(mComboLabel, 0, 3, 1, 1);
    mGrid.attach(mCombobox, 1, 3, 1, 1);
    mGrid.attach(mAddButton, 0, 4, 2, 1);


    mWordInput.set_placeholder_text("Put one word here");
    mWordInput.set_text(word);
    mWordInput.set_hexpand();
    mWordInput.signal_changed().connect([this]() { this->check_validity(); });

    // TODO inform further about missing translation text
    // maybe pull out logic for setting backround on button into member function


    mTranslationInput.set_placeholder_text("translation goes here");
    mTranslationInput.set_hexpand();
    mTranslationInput.signal_changed().connect(
        [this]() { this->check_validity(); });


    mAddButton.signal_clicked().connect([this]() {
            //TODO fix mLastDictForNewWord for something else
        auto dict = find_if(
            mLogic.mDicts.begin(), mLogic.mDicts.end(), [this](auto& d) {
                return d.getFilename() == mLogic.mLastDictForNewWord;
            });
        if (dict != mLogic.mDicts.end())
        {
            if (dict->addWord(
                    mWordInput.get_text(), mTranslationInput.get_text()))
            {
                this->hide();
            }
        }
        else
            cout<<"dict "<<mLogic.mLastDictForNewWord<<" not found"<<endl;
    });

    // create model for combobox with Dict selection
    {
        mRefListStore = Gtk::ListStore::create(mDictModel);
        mCombobox.set_model(mRefListStore);
        mRefListStore->clear();
        mCombobox.set_hexpand(true);
        mCombobox.pack_start(mDictModel.mPath);

        for (auto&& dict : logic.mDicts)
        {
            Gtk::TreeModel::iterator iter = mRefListStore->append();
            Gtk::TreeModel::Row row = *iter;
            row[mDictModel.mPath] = dict.getFilename();

            // select last selected
            if (mLogic.mLastDictForNewWord == dict.getFilename())
                mCombobox.set_active(row);
        }
        // if nothing selected, select first
        if ((mCombobox.get_active_row_number() == -1) &&
            (mLogic.mDicts.size() > 0))
            mCombobox.set_active(0);

        mCombobox.signal_changed().connect([this]() {
            auto iter = mCombobox.get_active();
            if (iter)
            {
                auto row = *iter;
                if (row)
                {
                    Glib::ustring name = row[mDictModel.mPath];
                    mLogic.mLastDictForNewWord = name;
                }
            }
        });
    }


    this->check_validity();
    this->show_all_children();
}
//------------------------------------------------------------------------------
NewWordWindow::~NewWordWindow()
{
}
//------------------------------------------------------------------------------
bool NewWordWindow::on_key_press_event(GdkEventKey* key_event)
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
void NewWordWindow::check_validity()
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
    std::string t2 = mTranslationInput.get_text();
    if (std::all_of(
            t2.begin(), t2.end(), [](auto& p) { return std::isspace(p); }))
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
        mAddButton.set_label("Add");
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
