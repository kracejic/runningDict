#pragma once

#include "Dict.h"
#include "Search.h"
#include "Processer.h"
#include "SpeedTimer.h"


//TODO temporary
template <class T> void ignore_arg(const T &) {}

class Logic
{
  public:
    Logic(int argc, char *argv[])
    {
        loadConfig();
        refreshAvailableFiles();


        ignore_arg(argc);
        ignore_arg(argv);
    };
    ~Logic(){
        saveConfig();
    };

    int mSizeX{500}, mSizeY{300};
    int mPositionX{0}, mPositionY{0};

    bool mTranslateClipboardAtStart{false};


    void refreshAvailableFiles();
    std::vector<Dict> mDicts;

    void loadConfig();
    void saveConfig();
};