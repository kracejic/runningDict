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
    Logic(int argc, char* argv[])
    {
        try
        {
            loadConfig();
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error during load configuration file: " << e.what()
                      << '\n';
        }

        try
        {
            refreshAvailableFiles();
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error during serching for new dictionaries: "
                      << e.what() << '\n';
        }


        ignore_arg(argc);
        ignore_arg(argv);
    };
    ~Logic()
    {
        saveConfig();
    };

    int mSizeX{500}, mSizeY{300};
    int mPositionX{0}, mPositionY{0};

    bool mTranslateClipboardAtStart{false};
    bool mAlwaysOnTop{true};

    std::string mLastDictForNewWord{""};


    void refreshAvailableFiles();
    std::vector<Dict> mDicts;

    void loadConfig();
    void saveConfig();
};