#include "SettingsWindow.h"
#include "Logic.h"
#include <string>

SettingsWindow::SettingsWindow(Logic& logic)
    : mLogic(logic)
    , mToogleFirstCatch("Translate clipboard at start")
    , mToogleAlwaysOnTop("Main window stays always on top")
{
    set_border_width(10);
    set_default_size(900, 400);
    add(mGrid);
    mGrid.attach(mToogleFirstCatch, 0,0,1,1);
    if (mLogic.mTranslateClipboardAtStart)
        mToogleFirstCatch.set_active();

    mGrid.attach(mToogleAlwaysOnTop, 0,1,1,1);
    if (mLogic.mAlwaysOnTop)
        mToogleAlwaysOnTop.set_active();


    //setup scrollView
    mGrid.attach(mScrollView, 0,6,10,1);
    mScrollView.set_hexpand();
    mScrollView.set_vexpand();
    mScrollView.set_policy(Gtk::PolicyType::POLICY_AUTOMATIC,
                           Gtk::PolicyType::POLICY_ALWAYS);
    mScrollView.add(mTreeView);
    mScrollView.set_margin_top(10);
    mScrollView.set_min_content_width(400);
    mScrollView.set_min_content_height(200);


    //dict treeView
    mRefListStore = Gtk::ListStore::create(mDictViewModel);
    mTreeView.set_model(mRefListStore);
    mTreeView.set_hexpand();
    mTreeView.set_vexpand();

    //fill treeview
    mTreeView.append_column_editable("Enabled", mDictViewModel.mEnabled);
    mTreeView.append_column("Path", mDictViewModel.mPath);
    mTreeView.get_column(1)->set_expand();
    mTreeView.append_column_editable("Priority", mDictViewModel.mBonus);


    //deal with enabling of dicts
    static_cast<Gtk::CellRendererToggle *>(
        mTreeView.get_column(0)->get_first_cell())
        ->signal_toggled().connect([this](const std::string &path)
        {
            try
            {
                int index = std::stoi(path);
                mLogic.mDicts.at(index).toogle_enable();
            }
            catch(const std::exception& e)
                {std::cerr << e.what() << '\n';}
        });

    //deal with changing prio of dicts
    static_cast<Gtk::CellRendererToggle *>(
        mTreeView.get_column(2)->get_first_cell())
        ->signal_toggled().connect([this](const std::string &path)
        {
            try
            {
                int index = std::stoi(path);
                if(mLogic.mDicts.at(index).mBonus<0)
                    mLogic.mDicts.at(index).mBonus = 0;
                else
                    mLogic.mDicts.at(index).mBonus = -1;
            }
            catch(const std::exception& e)
                {std::cerr << e.what() << '\n';}
        });


    // fill
    mRefListStore->clear();
    for(auto &&dict : logic.mDicts)
    {
        Gtk::TreeModel::iterator iter = mRefListStore->append();
        Gtk::TreeModel::Row row = *iter;
        row[mDictViewModel.mEnabled] = dict.is_enabled();
        row[mDictViewModel.mPath] = dict.getFilename();
        row[mDictViewModel.mBonus] = (dict.mBonus < 0);


    }

    this->show_all_children();
}
//------------------------------------------------------------------------------
SettingsWindow::~SettingsWindow()
{
    mLogic.mTranslateClipboardAtStart = mToogleFirstCatch.get_active();
    mLogic.mAlwaysOnTop = mToogleAlwaysOnTop.get_active();
}
//------------------------------------------------------------------------------
