#include "./Logic.h"
#include <experimental/filesystem>
#include <iostream>
#include <algorithm>
#include "json.hpp"
#include <fstream>

using namespace std;

namespace fs = std::experimental::filesystem;

using json = nlohmann::json;


void Logic::refreshFiles()
{

    //recursively go through child directories and find .dict files
    //Add them if they are not added
    int safetyNum = 0;
    for(const auto &p : fs::recursive_directory_iterator(
            fs::current_path(), fs::directory_options::skip_permission_denied))
    {
        //early return if structure is too deep
        if(++safetyNum > 1000)
            break;

        if(p.path().has_extension() == true && p.path().extension() == ".dict" )
        {
            if(none_of(mDicts.begin(), mDicts.end(), [&p](auto &val)
                    {return (val.second.getFilename() == p.path().string()); }))
            {
                mDicts.emplace_back(0, p.path().string());
                std::cout<<"Found new dict = "<<p<<std::endl;
            }
        }
    }
}
//------------------------------------------------------------------------------
void Logic::loadConfig()
{
    ifstream cfg_file("./config.json");

    if(cfg_file)
    {
        cout<<"Loading config"<<endl;
        json cfg;
        cfg_file >> cfg;

        if(cfg.count("test"))
            cout<<"xx = "<<cfg["test"]<<endl;

        cout << std::setw(4) << cfg <<endl;
    }
}
//------------------------------------------------------------------------------
void Logic::saveConfig()
{

}
//------------------------------------------------------------------------------