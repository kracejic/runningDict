#pragma once
#include <chrono>
#include <future>
#include <string>

#include "Dict.h"
#include "Processer.h"
#include "Search.h"
#include "SpeedTimer.h"


// TODO temporary
template <class T>
void ignore_arg(const T&)
{
}

enum class ServerStatus
{
    offline,
    serverNotAvailable,
    serverError,
    connecting,
    connected,
    synchronizing
};

class Logic
{
  public:
    Logic();
    ~Logic()
    {
        if (not mConfigFilename.empty())
            saveConfig(mConfigFilename);
    };

    int mSizeX{500}, mSizeY{300};
    int mPositionX{0}, mPositionY{0};

    bool mTranslateClipboardAtStart{false};
    bool mAlwaysOnTop{true};
    bool mDebug{false}; //< is debug logging on


    // clang-format off
    void setServer(const std::string& _mServer) {
        mServerStatus = ServerStatus::offline; mServer = _mServer; };
    const std::string& getServer() { return mServer; };
    // clang-format on

    /// last used dictionary for creating new word
    std::string mLastDictForNewWord{""};

  private:
    std::vector<Dict> mDicts;
    std::mutex dictsLock;

    /// url of the server
    std::string mServer{""};

  public:
    struct LockedDicts
    {
        std::vector<Dict>& dicts;
        std::unique_lock<std::mutex> lock;
    };
    LockedDicts getDicts()
    {
        return LockedDicts{mDicts, std::unique_lock<std::mutex>(dictsLock)};
    };


    std::vector<std::string> mAdditionalSearchDirs;

    /// Returns pointer to dict or zero, only filename is checked (not
    /// extension)
    Dict* getDict(const std::string& name);
    /// Loads config from default positions
    bool initWithConfig();
    /// Loads config from parameter
    bool initWithConfig(const std::string& filename);
    /// Checks all usual directories for dictionaries
    void refreshAvailableDicts();

    static std::string getConfigPath();
    std::string getPackagePath();

    bool createDict(const std::string& filename, bool online);

    std::future<void> connectToServerAndSync(const std::string& url);
    std::future<void> connectToServerAndSync();
    std::future<void> connectToServerAndSyncIfItIsNeccessary();


    ServerStatus mServerStatus{ServerStatus::offline};
    std::chrono::system_clock::time_point mLastServerSync;

  private:
    std::string mConfigFilename;
    std::string mConfigDir;

    /// checks dictionaries in directory
    void loadDictsInDir(const std::string& path);

    void loadConfig(const std::string& filename);
    void saveConfig(const std::string& filename);
};
