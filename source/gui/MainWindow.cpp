#include "./MainWindow.h"
#include "../Search.h"
#include "../Processer.h"

#include <iostream>
#include <gdkmm/rgba.h>
#include <gtk/gtk.h>
#include "SettingsWindow.h"

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

    // settings button clicked shows settings window
    mSettingsButton.signal_clicked().connect(
        [this](){
            SettingsWindow *setings;
            setings = new SettingsWindow(mLogic);
            setings->show();

            //refresh on settings closed
            setings->signal_hide().connect([this](){
                    this->executeSearch(mWordInput.get_text());
                });
        });


    add(mGrid);

    mGrid.add(mWordInput);
    mGrid.add(mAddWordButton);
    mGrid.add(mSettingsButton);

    //scrolling area
    mGrid.attach(mScrollForResults, 0,2,10,1);
    mScrollForResults.set_hexpand();
    mScrollForResults.set_policy(Gtk::PolicyType::POLICY_AUTOMATIC,
                                 Gtk::PolicyType::POLICY_ALWAYS);
    mScrollForResults.add(mTreeView);
    mScrollForResults.set_margin_top(10);
    mScrollForResults.set_min_content_width(400);
    mScrollForResults.set_min_content_height(200);

    //results treeView
    mRefListStore = Gtk::ListStore::create(mColumns);
    mTreeView.set_model(mRefListStore);
    mTreeView.set_hexpand();
    mTreeView.set_vexpand();

    //Word Culumn
    {
        mTreeView.append_column("Word", mColumns.mGerman);
        Gtk::TreeViewColumn *pColumn = mTreeView.get_column(0);
        pColumn->set_sizing(Gtk::TreeViewColumnSizing::TREE_VIEW_COLUMN_AUTOSIZE);
        Gdk::Color col("#ffaa00");
        static_cast<Gtk::CellRendererText *>(pColumn->get_first_cell())
            ->property_foreground_gdk()
            .set_value(col);
    }
    //Match Culumn
    {
        mTreeView.append_column("Match", mColumns.mGerman_found);
        Gtk::TreeViewColumn *pColumn = mTreeView.get_column(1);
        pColumn->set_sizing(Gtk::TreeViewColumnSizing::TREE_VIEW_COLUMN_AUTOSIZE);
        pColumn->set_cell_data_func(
            *pColumn->get_first_cell(),
            [this](Gtk::CellRenderer *renderer,
                   const Gtk::TreeModel::iterator &iter)
            {
                Gtk::TreeModel::Row row = *iter;
                Gdk::Color col("#ff0000");
                auto score = row[this->mColumns.mScore];
                if(score < 0)
                    score = 0;
                if(score > 4)
                    score = 4;
                switch (score)
                {
                    case 0:
                        col.set("#40B640");
                        break;
                    case 1:
                        col.set("#82B640");
                        break;
                    case 2:
                        col.set("#AFB640");
                        break;
                    case 3:
                        col.set("#B67E40");
                        break;
                    case 4:
                        col.set("#B64640");
                        break;
                    default:
                        break;
                }
                static_cast<Gtk::CellRendererText *>(renderer)
                    ->property_foreground_gdk()
                    .set_value(col);
            });
    }
    //Translation Culumn
    {
        mTreeView.append_column("Translation", mColumns.mEnglish);
        Gtk::TreeViewColumn* pColumn = mTreeView.get_column(2);
        pColumn->set_sizing(Gtk::TreeViewColumnSizing::TREE_VIEW_COLUMN_AUTOSIZE);
        static_cast<Gtk::CellRendererText *>(pColumn->get_first_cell())
            ->property_wrap_mode().set_value(Pango::WRAP_WORD_CHAR);
    }
    // mTreeView.append_column("Score", mColumns.mScore);

    //deal with resizing
    this->signal_check_resize().connect([this]()
    {
        //calculate remaining size
        Gtk::TreeViewColumn* pColumn = mTreeView.get_column(2);
        auto width = this->get_allocated_width()
            - mTreeView.get_column(0)->get_width()
            - mTreeView.get_column(1)->get_width()-30;

        //minimum reasonable size for column
        if(width < 150)
            width = 150;

        static_cast<Gtk::CellRendererText *>(pColumn->get_first_cell())
            ->property_wrap_width().set_value(width);

        //debounce
        static auto oldsize = 0;
        if(oldsize != width)
        {
            oldsize = width;

            //trigger redraw of mTreeView
            unique_lock<mutex> guard{this->mSearchMutex};
            this->mRedrawNeeded = true;
        }
    });


    // mWordInput.set_text("Put here text to translation...");
    mWordInput.set_text("Das ist einee CKatze Katzeee abbestellt begeststellenai...");
    mWordInput.set_hexpand();

    this->show_all_children();

    //make pulse called repeatedly every 100ms
    sigc::slot<bool> my_slot = sigc::bind(
        sigc::mem_fun(*this, &MainWindow::pulse), 0);
    mPulseConnection = Glib::signal_timeout().connect(my_slot, 150);
}
//------------------------------------------------------------------------------
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
    ignore_arg(num);

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
    if(mNewTranslationAvailable || mRedrawNeeded)
    {
        mRefListStore->clear();

        //for all words, push results to ListStore
        for(auto &&w : mTranslationWords)
        {
            auto &rr = mTranslationResult[w];
            bool first = true;
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
            }
            //if no match found, still display atleast the word
            if (first){
                Gtk::TreeModel::iterator iter = mRefListStore->append();
                (*iter)[mColumns.mGerman] = w;
            }
        }
        mNewTranslationAvailable = false;
        mRedrawNeeded = false;
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
