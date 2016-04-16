#include "Dict.h"
#include "Search.h"
#include "Processer.h"
#include "SpeedTimer.h"

template <class T> void ignore(const T &) {}

class Logic
{
  public:
    Logic(int argc, char *argv[])
    {
        // TODO load settings first
        loadConfig();

        refreshFiles();
        refreshFiles(); // TODO delete this
        ignore(argc);
        ignore(argv);
    };
    ~Logic(){
        saveConfig();
    };

    void refreshFiles();
    std::vector<std::pair<int, Dict>> mDicts;

    void loadConfig();
    void saveConfig();
};