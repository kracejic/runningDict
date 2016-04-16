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
    for(const auto &file : fs::recursive_directory_iterator(
            fs::current_path(), fs::directory_options::skip_permission_denied))
    {
        //early return if structure is too deep
        if(++safetyNum > 1000)
            break;

        if(file.path().has_extension() == true && file.path().extension() == ".dict" )
        {
            if(none_of(mDicts.begin(), mDicts.end(), [&file](auto &val)
                {
                    return (fs::equivalent(val.second.getFilename(), file.path()));
                }))
            {
                mDicts.emplace_back(0, file.path().string());
                std::cout<<"Found new dict: "<<file<<std::endl;
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

        //load dictionaries
        if(cfg.count("dicts") && cfg["dicts"].is_array()){
            for (json& dict : cfg["dicts"])
            {
                string filename = dict[1];
                if( fs::exists(filename) )
                {
                    cout << "importing new dict: " << filename << endl;
                    mDicts.emplace_back(dict[0], filename);
                }
            }
        }


        // cout << std::setw(4) << cfg <<endl;
    }
}
//------------------------------------------------------------------------------
void Logic::saveConfig()
{
    //prepare json object with settings
    cout<<"Saving config"<<endl;
    json cfg;
    for(auto& dict : mDicts)
        cfg["dicts"].push_back({dict.first, dict.second.getFilename()});

    //save settings
    ofstream outFile("./config.json");
    outFile << std::setw(4) << cfg << endl;
}
//------------------------------------------------------------------------------