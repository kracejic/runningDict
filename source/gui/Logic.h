#include "Dict.h"
#include "Search.h"
#include "Processer.h"
#include "SpeedTimer.h"


//TODO temporary
template <class T> void ignore(const T &) {}

class Logic
{
  public:
    Logic(int argc, char *argv[])
    {
        loadConfig();
        refreshAvailableFiles();


        ignore(argc);
        ignore(argv);
    };
    ~Logic(){
        saveConfig();
    };

    int mSizeX{500}, mSizeY{300};
    int mPositionX{0}, mPositionY{0};


    void refreshAvailableFiles();
    std::vector<std::pair<int, Dict>> mDicts;

    void loadConfig();
    void saveConfig();
};