#include "SettingsWindow.h"
#include "Logic.h"

SettingsWindow::SettingsWindow(Logic& logic)
    : mLogic(logic)
{
    set_border_width(10);
    set_default_size(900, 400);
    add(mScrollView);

    //setup scrollView
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
        ->signal_toggled().connect([](const auto &path)
        {
            //TODO implement
            std::cout << "path = " << path << std::endl;
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

}
//------------------------------------------------------------------------------
