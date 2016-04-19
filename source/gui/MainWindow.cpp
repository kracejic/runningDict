#include "./MainWindow.h"

#include <iostream>

using namespace std;

MainWindow::MainWindow(Logic& logic)
    : mLogic(logic)
    , mAddWordButton("+")
    , mSettingsButton("O")
{
    // Sets the border width of the window.
    set_border_width(10);
    set_default_size(500, 250);

    // When the button receives the "clicked" signal, it will call the
    // on_button_clicked() method defined below.
    // mbutton.signal_clicked().connect(sigc::memfun(*this,
    //             &MainWindow::on_button_clicked));


    add(mGrid);

    mGrid.add(mWordInput);
    mGrid.add(mAddWordButton);
    mGrid.add(mSettingsButton);


    mWordInput.set_text("Put here text to translation...");
    mWordInput.set_hexpand();

    this->show_all_children();

    //make pulse repeated every 100ms
    sigc::slot<bool> my_slot = sigc::bind(
        sigc::mem_fun(*this, &MainWindow::pulse), 0);
    mPulseConnection = Glib::signal_timeout().connect(my_slot, 150);
}
//-----------------------------------------------------------------------------------
bool MainWindow::pulse(int num)
{

    //check if text has changed
    string tmp = mWordInput.get_text();
    if( mOldTextInEntry != tmp)
    {
        mOldTextInEntry = tmp;
        std::cout << "changed..." << std::endl;
        //todo max size of the text

        executeSearch(tmp);
    }

    Glib::RefPtr<Gtk::Clipboard> refClipboard = Gtk::Clipboard::get();
    refClipboard->request_contents("UTF8_STRING",
    sigc::mem_fun(*this, &MainWindow::on_clipboard_received) );

    // string clip =

    return true;
}
//------------------------------------------------------------------------------
void MainWindow::executeSearch(string text)
{
    unique_lock<mutex> guard(mSearchMutex);
    mWaitingToTranslate = text;

    if(!mSearchInProgress)
    {
        //TODO run in searchThread
        mSearchInProgress = true;
    }

    guard.unlock();

}
//------------------------------------------------------------------------------
void MainWindow::searchThread()
{
    string text;

    //load text
    unique_lock<mutex> guard(mSearchMutex);

    while(true)
    {
        text = mWaitingToTranslate;
        mWaitingToTranslate = "";
        guard.unlock();

        guard.lock();
        mSearchInProgress = false;
        if(mWaitingToTranslate == "")
            break;
    }

    mSearchInProgress = false;
}
//------------------------------------------------------------------------------
void MainWindow::on_clipboard_received(const Gtk::SelectionData &data)
{
    // check if new text is in the clipboard
    string text = data.get_data_as_string();
    if(mOldClipboard != text)
    {
        mOldClipboard = text;
        if(!mIgnoreClipboardChange)
        {
            std::cout << "Clipboard changed..." << std::endl;
            mWordInput.set_text(text);
            // we do not need to execute search, since pulse does that for us
            // when text is changed.
        }
        mIgnoreClipboardChange = false;
    }
}
//------------------------------------------------------------------------------
void MainWindow::on_button_clicked()
{
  std::cout << "Hello World" << std::endl;
}
