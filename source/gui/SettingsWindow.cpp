#include "SettingsWindow.h"
#include "Logic.h"
#include "version.h"
#include <string>

SettingsWindow::SettingsWindow(Logic& logic)
    : mLogic(logic)
    , mToogleFirstCatch("Translate clipboard at start")
    , mToogleAlwaysOnTop("Main window stays always on top")
{
    this->set_position(Gtk::WIN_POS_MOUSE);
    this->set_border_width(10);
    this->set_default_size(500, 400);
    this->add(mGrid);
    this->set_title("Settings");

    mGrid.attach(mToogleFirstCatch, 0, 0, 1, 1);
    if (mLogic.mTranslateClipboardAtStart)
        mToogleFirstCatch.set_active();

    mGrid.attach(mToogleAlwaysOnTop, 0, 1, 1, 1);
    if (mLogic.mAlwaysOnTop)
        mToogleAlwaysOnTop.set_active();

    mGrid.attach(mAddDictButton, 1, 0, 2, 2);
    mAddDictButton.set_label("New dict");
    mAddDictButton.signal_clicked().connect([this]() {
        if (mAddDictWindow)
            return;
        mAddDictWindow.reset(new NewDictWindow(mLogic));
        mAddDictWindow->show();

        // refresh on settings closed
        mAddDictWindow->signal_hide().connect([this]() {
            // delete mAddDictWindow;
            mAddDictWindow.reset(); // destructor is called
            this->refreshDicts();
        });
    });

    // setup scrollView
    mGrid.attach(mScrollView, 0, 6, 2, 1);
    mScrollView.set_hexpand();
    mScrollView.set_vexpand();
    mScrollView.set_policy(
        Gtk::PolicyType::POLICY_AUTOMATIC, Gtk::PolicyType::POLICY_ALWAYS);
    mScrollView.add(mTreeView);
    mScrollView.set_margin_top(10);
    mScrollView.set_min_content_width(400);
    mScrollView.set_min_content_height(200);

    mGrid.attach(mWebSiteLabel, 0, 8, 1, 1);
    mWebSiteLabel.set_hexpand();
    mWebSiteLabel.set_margin_top(10);
    mWebSiteLabel.set_text("https://github.com/kracejic/runningDict");

    mGrid.attach(mVersionLabel, 1, 8, 1, 1);
    mVersionLabel.set_hexpand();
    mVersionLabel.set_margin_top(10);
    mVersionLabel.set_text(Version::getVersionShort());

    // dict treeView
    mRefListStore = Gtk::ListStore::create(mDictViewModel);
    mTreeView.set_model(mRefListStore);
    mTreeView.set_hexpand();
    mTreeView.set_vexpand();

    // fill treeview
    mTreeView.append_column_editable("Enabled", mDictViewModel.mEnabled);
    mTreeView.append_column("Path", mDictViewModel.mPath);
    mTreeView.get_column(1)->set_expand();
    mTreeView.append_column_editable("Priority", mDictViewModel.mBonus);
    mTreeView.set_tooltip_column(mDictViewModel.mTooltip.index());


    Gtk::TreeViewColumn* pColumn = mTreeView.get_column(1);
    pColumn->set_cell_data_func(
        *pColumn->get_first_cell(), [this](Gtk::CellRenderer* renderer,
                                        const Gtk::TreeModel::iterator& iter) {
            Gtk::TreeModel::Row row = *iter;
            if (row[this->mDictViewModel.mError])
            {
                Gdk::Color col("#B65E40");
                dynamic_cast<Gtk::CellRendererText*>(renderer)
                    ->property_foreground_gdk()
                    .set_value(col);
            }

        });

    // deal with enabling of dicts
    dynamic_cast<Gtk::CellRendererToggle*>(
        mTreeView.get_column(0)->get_first_cell())
        ->signal_toggled()
        .connect([this](const std::string& path) {
            try
            {
                int index = std::stoi(path);
                mLogic.mDicts.at(index).toogle_enable();
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        });

    // deal with changing prio of dicts
    dynamic_cast<Gtk::CellRendererToggle*>(
        mTreeView.get_column(2)->get_first_cell())
        ->signal_toggled()
        .connect([this](const std::string& path) {
            try
            {
                int index = std::stoi(path);
                if (mLogic.mDicts.at(index).mBonus < 0)
                    mLogic.mDicts.at(index).mBonus = 0;
                else
                    mLogic.mDicts.at(index).mBonus = -1;
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        });


    this->refreshDicts();

    this->show_all_children();
}
//------------------------------------------------------------------------------
SettingsWindow::~SettingsWindow()
{
    mLogic.mTranslateClipboardAtStart = mToogleFirstCatch.get_active();
    mLogic.mAlwaysOnTop = mToogleAlwaysOnTop.get_active();
}
//------------------------------------------------------------------------------
void SettingsWindow::refreshDicts()
{
    mRefListStore->clear();
    for (auto&& dict : mLogic.mDicts)
    {
        Gtk::TreeModel::iterator iter = mRefListStore->append();
        Gtk::TreeModel::Row row = *iter;
        row[mDictViewModel.mEnabled] = dict.is_enabled();
        row[mDictViewModel.mPath] = dict.getName();
        row[mDictViewModel.mBonus] = (dict.mBonus < 0);
        row[mDictViewModel.mError] = dict.mErrorState;
        row[mDictViewModel.mTooltip] = dict.getFilename();
    }
}
//-----------------------------------------------------------------------------
bool SettingsWindow::on_key_press_event(GdkEventKey* key_event)
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
