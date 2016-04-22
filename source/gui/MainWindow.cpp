#include "./MainWindow.h"
#include "../Search.h"
#include "../Processer.h"

#include <iostream>
#include <gdkmm/rgba.h>

using namespace std;

MainWindow::MainWindow(Logic& logic)
    : mLogic(logic)
    , mAddWordButton("+")
    , mSettingsButton("O")
{
    // Sets the border width of the window.
    set_border_width(10);
    set_default_size(mLogic.mSizeX, mLogic.mSizeY);
    this->move(mLogic.mPositionX, mLogic.mPositionY);
    set_keep_above(true);

    // When the button receives the "clicked" signal, it will call the
    // on_button_clicked() method defined below.
    // mbutton.signal_clicked().connect(sigc::memfun(*this,
    //             &MainWindow::on_button_clicked));


    add(mGrid);

    mGrid.add(mWordInput);
    mGrid.add(mAddWordButton);
    mGrid.add(mSettingsButton);
    mGrid.attach(mResultGrid, 0,1,10,1);

    Gdk::RGBA col;
    // col.set_rgba(1,0.8,1);
    // mResultGrid.override_background_color(col);
    mResultGrid.set_vexpand();
    mResultGrid.set_hexpand();
    mResultGrid.add(mTreeView);
    mRefListStore = Gtk::ListStore::create(mColumns);
    mTreeView.set_model(mRefListStore);
    mTreeView.append_column("Word", mColumns.mGerman);
    mTreeView.append_column("Match", mColumns.mGerman_found);
    mTreeView.append_column("Translation", mColumns.mEnglish);
    mTreeView.append_column("Score", mColumns.mScore);

    // mWordInput.set_text("Put here text to translation...");
    mWordInput.set_text("Das ist eine CKatze...");
    mWordInput.set_hexpand();

    this->show_all_children();

    //make pulse repeated every 100ms
    sigc::slot<bool> my_slot = sigc::bind(
        sigc::mem_fun(*this, &MainWindow::pulse), 0);
    mPulseConnection = Glib::signal_timeout().connect(my_slot, 150);
}
//------------------------------------------------------------------------------
#include <gtk/gtk.h>
MainWindow::~MainWindow()
{
    // present refreshes window position
    this->present();

    //saves data to logic, this is then saved to json config file
    this->get_position(mLogic.mPositionX, mLogic.mPositionY);
    this->get_size(mLogic.mSizeX, mLogic.mSizeY);
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


    unique_lock<mutex> guard{mSearchMutex};
    if(mNewTranslationAvailable)
    {
        mRefListStore->clear();
        Gtk::TreeModel::iterator iter = mRefListStore->append();
        Gtk::TreeModel::Row row = *iter;
        row[mColumns.mGerman] = "ss";

        for(auto &&w : mTranslationWords)
        {
            auto &rr = mTranslationResult[w];
            bool first = true;
            // cout << w << endl;
            for(auto &&r : rr)
            {
                Gtk::TreeModel::iterator iter = mRefListStore->append();
                Gtk::TreeModel::Row row = *iter;
                if (first)
                    row[mColumns.mGerman] = w;
                first = false;
                row[mColumns.mGerman_found] = r.match;
                row[mColumns.mEnglish] = r.words;
                row[mColumns.mScore] = r.score;

        //         cout << "  " << r.score << ":" << r.match << " -" << r.words
        //              << endl;
            }
        }
        mNewTranslationAvailable = false;
    }


    return true;
}
//------------------------------------------------------------------------------
void MainWindow::executeSearch(string text)
{
    unique_lock<mutex> guard{mSearchMutex};
    mWaitingToTranslate = text;

    if(!mSearchInProgress)
    {
        //TODO run in searchThread
        mSearchInProgress = true;
        guard.unlock();
        std::thread thread {&MainWindow::searchThread, this};
        thread.detach();
    }

}
//------------------------------------------------------------------------------
void MainWindow::searchThread()
{
    string text;

    //load text
    unique_lock<mutex> guard(mSearchMutex);

    while(true)
    {
        //load string and set it to empty
        text = mWaitingToTranslate;
        mWaitingToTranslate = "";
        guard.unlock();

        //translate, during translation can Pulse add new string to translate
        int numthreads = std::thread::hardware_concurrency();
        numthreads = (numthreads > 1) ? numthreads : 1;
        std::vector<string> words = Processer::splitToWords(text.c_str());
        numthreads = 1; //TODO increase

        workerResult results = _search(mLogic.mDicts, numthreads, words, false);
        cout << "results are here" << endl;
        // for(auto &&w : words)
        // {
        //     auto &rr = results[w];
        //     cout << w << endl;
        //     for(auto &&r : rr)
        //     {
        //         cout << "  " << r.score << ":" << r.match << " -" << r.words
        //              << endl;
        //     }
        // }

        //lock and test if there is another string to translate
        guard.lock();
        mTranslationResult = results;
        mTranslationWords = words;
        mNewTranslationAvailable = true;
        if(mWaitingToTranslate == "")
            break;
    }

    cout<<"Finish"<<endl;
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
