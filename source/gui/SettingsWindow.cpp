#include "SettingsWindow.h"
#include "Logic.h"
#include "log.h"
#include "version.h"
#include <chrono>
#include <string>

using namespace std;

SettingsWindow::SettingsWindow(Logic& logic)
    : mLogic(logic)
    , mToogleFirstCatch("Translate clipboard at start")
    , mToogleAlwaysOnTop("Main window stays always on top")
    , mServerStatus("")
    , mToogleServer("Synchronization server")
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

    mGrid.attach(mAddDictButton, 3, 0, 1, 2);
    mAddDictButton.set_label("New dictionary");
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


    // server settings
    mGrid.attach(mServerSettingBox, 0, 3, 3, 1);
    // mServerSettingBox.set_margin_top(10);
    mServerSettingBox.pack_start(mToogleServer, false, false, 0);
    mServerSettingBox.pack_start(mServer, false, false, 10);
    mServerSettingBox.pack_start(mServerStatus, false, false, 0);
    mServerStatus.override_color(Gdk::RGBA{"#909090"});
    mServer.set_placeholder_text("url");
    if (mLogic.getServer() == "")
    {
        mServer.set_sensitive(false);
        mToogleServer.set_active(false);
    }
    else
    {
        mServer.set_sensitive(true);
        mToogleServer.set_active(true);
        mServer.set_text(mLogic.getServer());
    }
    mToogleServer.signal_toggled().connect([this]() {
        if (!mToogleServer.get_active())
        {
            mServer.set_sensitive(false);
            mServerStatus.set_text("");
            mServer.set_text("");
            mLogic.setServer("");
        }
        else
        {
            mServer.set_sensitive(true);
        }
    });

    // setup scrollView
    mGrid.attach(mScrollView, 0, 6, 4, 1);
    mScrollView.set_hexpand();
    mScrollView.set_vexpand();
    mScrollView.set_policy(
        Gtk::PolicyType::POLICY_AUTOMATIC, Gtk::PolicyType::POLICY_ALWAYS);
    mScrollView.add(mTreeView);
    mScrollView.set_margin_top(10);
    mScrollView.set_min_content_width(400);
    mScrollView.set_min_content_height(200);

    mGrid.attach(mWebSiteLabel, 1, 8, 1, 1);
    mWebSiteLabel.set_hexpand();
    mWebSiteLabel.set_margin_top(10);
    mWebSiteLabel.set_text("https://github.com/kracejic/runningDict");

    mGrid.attach(mVersionLabel, 2, 8, 2, 1);
    mVersionLabel.set_hexpand();
    mVersionLabel.set_margin_top(10);
    mVersionLabel.set_text(Version::getVersionShort());
    mVersionLabel.set_halign(Gtk::Align::ALIGN_END);

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
    mTreeView.append_column("Shared", mDictViewModel.mOnline);
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
                auto lockedD = mLogic.getDicts();
                int index = std::stoi(path);
                lockedD.dicts.at(index).toogle_enable();
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
                auto lockedD = mLogic.getDicts();
                int index = std::stoi(path);
                if (lockedD.dicts.at(index).mBonus < 0)
                    lockedD.dicts.at(index).mBonus = 0;
                else
                    lockedD.dicts.at(index).mBonus = -1;
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        });


    this->refreshDicts();

    this->show_all_children();

    // make pulse called repeatedly every 100ms
    sigc::slot<bool> my_slot =
        sigc::bind(sigc::mem_fun(*this, &SettingsWindow::pulse), 0);
    mPulseConnection = Glib::signal_timeout().connect(my_slot, 1000);
}
//------------------------------------------------------------------------------
SettingsWindow::~SettingsWindow()
{
    mLogic.mTranslateClipboardAtStart = mToogleFirstCatch.get_active();
    mLogic.mAlwaysOnTop = mToogleAlwaysOnTop.get_active();
}
//------------------------------------------------------------------------------
bool SettingsWindow::pulse(int num)
{
    ignore_arg(num);

    if (mToogleServer.get_active() &&
        (!mServerConnection.valid() ||
            mServerConnection.wait_for(1ns) == future_status::ready))
    {
        if (mLogic.getServer() != mServer.get_text())
            mLogic.setServer(mServer.get_text());
        mServerConnection = mLogic.connectToServerAndSync();
    }

    if (mToogleServer.get_active())
    {
        mServerStatus.set_text("");

        mServer.set_sensitive(true);
        // Compute last contact with server
        auto now = std::chrono::system_clock::now();
        int diff =
            chrono::duration_cast<chrono::seconds>(now - mLogic.mLastServerSync)
                .count();

        string textDiff = to_string(diff) + "seconds ago";
        if (diff > 60)
            textDiff = to_string(diff / 60) + "minutes ago";
        if (diff > 3600)
            textDiff = to_string(diff / 3600) + "hours ago";
        if (diff > 3600 * 24 + 365)
            textDiff = "not available";

        mServerStatus.set_tooltip_text(
            "Last contact with server was: " + textDiff);

        switch (mLogic.mServerStatus)
        {
            case ServerStatus::offline:
                mServerStatus.set_text("offline");
                break;
            case ServerStatus::serverNotAvailable:
                mServerStatus.set_text("server offline");
                break;
            case ServerStatus::serverError:
                mServerStatus.set_text("server error");
                break;
            case ServerStatus::connecting:
                mServerStatus.set_text("connecting");
                break;
            case ServerStatus::connected:
                mServerStatus.set_text("connected");
                break;
            case ServerStatus::synchronizing:
                mServerStatus.set_text("synchronizing");
                break;
        }
    }
    else
    {
        mServer.set_sensitive(false);
        mServerStatus.set_text("");
        mServer.set_text("");
        mLogic.setServer("");
    }

    return true;
}
//-----------------------------------------------------------------------------
void SettingsWindow::refreshDicts()
{
    mRefListStore->clear();
    auto lockedD = mLogic.getDicts();
    for (auto&& dict : lockedD.dicts)
    {
        Gtk::TreeModel::iterator iter = mRefListStore->append();
        Gtk::TreeModel::Row row = *iter;
        row[mDictViewModel.mEnabled] = dict.isEnabled();
        row[mDictViewModel.mPath] = dict.getName();
        row[mDictViewModel.mBonus] = (dict.mBonus < 0);
        row[mDictViewModel.mOnline] = dict.mOnline;
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
