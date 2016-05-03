#include "./Logic.h"
#include <experimental/filesystem>
#include <iostream>
#include <algorithm>
#include "json.hpp"
#include <fstream>

using namespace std;

namespace fs = std::experimental::filesystem;

using json = nlohmann::json;


/**
 * Returns relative path from *from* to *to*.
 */
fs::path relativeTo(fs::path from, fs::path to)
{
   // Start at the root path and while they are the same then do nothing then when they first
   // diverge take the remainder of the two path and replace the entire from path with ".."
   // segments.
   fs::path::const_iterator fromIter = from.begin();
   fs::path::const_iterator toIter = to.begin();

   // Loop through both
   while (fromIter != from.end() && toIter != to.end() && (*toIter) == (*fromIter))
   {
      ++toIter;
      ++fromIter;
   }

   fs::path finalPath;
   while (fromIter != from.end())
   {
      finalPath /= "..";
      ++fromIter;
   }

   while (toIter != to.end())
   {
      finalPath /= *toIter;
      ++toIter;
   }

   return finalPath;
}


void Logic::refreshAvailableFiles()
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
                    return (fs::equivalent(val.getFilename(), file.path()));
                }))
            {
                auto relPath = relativeTo(fs::current_path(), file.path());
                mDicts.emplace_back(relPath.string(), 0, false);
                std::cout<<"Found new dict: "<<file<<std::endl;
                std::cout<<"      rel path: "<<relPath.string()<<std::endl;
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
                string filename = dict[0];
                if( fs::exists(filename) )
                {
                    cout << "importing new dict: " << filename << endl;
                    mDicts.emplace_back(filename, dict[1], dict[2]);
                }
            }
        }

        if(cfg.count("position") && cfg["position"].size() == 2)
        {
            cout<<"loading position"<<endl;
            mPositionX = cfg["position"][0];
            mPositionY = cfg["position"][1];
        }
        if(cfg.count("size") && cfg["size"].size() == 2)
        {
            cout<<"loading size"<<endl;
            mSizeX = cfg["size"][0];
            mSizeY = cfg["size"][1];
        }
        if(cfg.count("translateClipboardAtStart") > 0)
            mTranslateClipboardAtStart
                = cfg["translateClipboardAtStart"];


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
        cfg["dicts"].push_back(
            {dict.getFilename(), dict.mBonus, dict.is_enabled()});

    cfg["size"] = {mSizeX, mSizeY};
    cfg["position"] = {mPositionX, mPositionY};
    cfg["translateClipboardAtStart"] = mTranslateClipboardAtStart;

    //save settings
    ofstream outFile("./config.json");
    outFile << std::setw(4) << cfg << endl;
}
//------------------------------------------------------------------------------