#include "NewWordWindow.h"
#include <algorithm>
#include <cctype>
#include <string>

using namespace std;

NewWordWindow::NewWordWindow(Logic &logic)
: mLogic(logic)
, mAddButton("Add")
{
    this->add(mGrid);
    this->set_default_size(400,-1);

    set_border_width(10);

    mGrid.set_column_spacing(5);
    mGrid.set_row_spacing(5);

    mGrid.attach(mWordInput, 0,1,1,1);
    mGrid.attach(mTranslationInput, 0,2,2,1);
    mGrid.attach(mAddButton, 0,3,2,1);





    mWordInput.set_placeholder_text("Put one word here");
    mWordInput.set_text("word");
    mWordInput.set_hexpand();
    mWordInput.signal_changed().connect([this](){
        std::string t = mWordInput.get_text();

        //check for spaces in entry
        if(std::find_if(t.begin(), t.end(),
                        [](auto &p)
                        {
                            return std::isspace(p);
                        })
           == t.end())
        {
            //word is OK
            mWordInput.unset_color();
            mAddButton.unset_background_color();
            mAddButton.set_label("Add");
            mAddButton.set_sensitive(true);
        }
        else
        {
            Gdk::RGBA col("#D62B0B");
            Gdk::RGBA col2("#E5AFA5");
            mWordInput.override_color(col);
            mWordInput.queue_compute_expand();
            mAddButton.set_label("Translated word must be without spaces.");
            mAddButton.override_background_color(col2);
            mAddButton.set_sensitive(false);
        }
    });

    //TODO inform further about missing translation text
    //maybe pull out logic for setting backround on button into member function


    mTranslationInput.set_placeholder_text("translation goes here");
    mTranslationInput.set_hexpand();

    this->show_all_children();
}
//------------------------------------------------------------------------------
NewWordWindow::~NewWordWindow()
{

}
//------------------------------------------------------------------------------
bool NewWordWindow::on_key_press_event(GdkEventKey* key_event)
{
    if(key_event->keyval == GDK_KEY_Escape)
    {
        // close the window, when the 'esc' key is pressed
        hide();
        return true;
    }

    return Gtk::Window::on_key_press_event(key_event);
}

