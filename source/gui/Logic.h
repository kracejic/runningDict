#pragma once
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

class Logic
{
  public:
    Logic(){};
    ~Logic()
    {
        if (not mFilename.empty())
            saveConfig(mFilename);
    };

    int mSizeX{500}, mSizeY{300};
    int mPositionX{0}, mPositionY{0};

    bool mTranslateClipboardAtStart{false};
    bool mAlwaysOnTop{true};

    std::string mLastDictForNewWord{""};

    std::string mFilename;


    bool initWithConfig();
    bool initWithConfig(const std::string& filename);
    void refreshAvailableDicts();
    void loadDictsInDir(const std::string& path);
    std::vector<Dict> mDicts;
    Dict* getDict(const std::string& name);

    void loadConfig(const std::string& filename);
    void saveConfig(const std::string& filename);
};
