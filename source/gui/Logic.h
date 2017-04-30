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
    bool mDebug{false};

    std::string mServer{""};

    std::string mLastDictForNewWord{""};
    std::vector<Dict> mDicts;
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

    bool createDict(const std::string& filename);

    std::future<void> connectToServerAndSync(const std::string& url);
    std::future<void> connectToServerAndSync();

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
