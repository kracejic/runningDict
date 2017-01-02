#include "./Logic.h"
#include "json.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>


#ifdef USE_BOOST_FILESYSTEM
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

using namespace std;
using json = nlohmann::json;


/**
 * Returns relative path from *from* to *to*.
 */
fs::path relativeTo(const fs::path& from, const fs::path& to)
{
    // Start at the root path and while they are the same then do nothing then
    // when they first
    // diverge take the remainder of the two path and replace the entire from
    // path with ".."
    // segments.
    fs::path::const_iterator fromIter = from.begin();
    fs::path::const_iterator toIter = to.begin();

    // Loop through both
    while (fromIter != from.end() && toIter != to.end() &&
           (*toIter) == (*fromIter))
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

//-----------------------------------------------------------------------------
void Logic::refreshAvailableDicts()
{
    auto path = fs::current_path() / ".." / "share" / "runningDict";
    this->loadDictsInDir(path);
}
//-----------------------------------------------------------------------------
void Logic::loadDictsInDir(const std::string& path)
{

    // recursively go through child directories and find .dict files
    // Add them if they are not added
    int safetyNum = 0;

#ifdef USE_BOOST_FILESYSTEM
    for (const auto& file : fs::recursive_directory_iterator(fs::path(path)))
#else
    for (const auto& file : fs::recursive_directory_iterator(
             fs::path(path), fs::directory_options::skip_permission_denied))
#endif
    {
        // early return if structure is too deep
        if (++safetyNum > 10000)
            break;

        if (file.path().has_extension() && file.path().extension() == ".dict")
        {
            if (none_of(mDicts.begin(), mDicts.end(), [&file](auto& val) {
                    return (fs::equivalent(val.getFilename(), file.path()));
                }))
            {
                auto relPath = relativeTo(fs::current_path(), file.path());
                mDicts.emplace_back(relPath.string(), 0, false);
                std::cout << "Found new dict: " << file << std::endl;
                std::cout << "      rel path: " << relPath.string()
                          << std::endl;
            }
        }
    }
}
//-----------------------------------------------------------------------------
Dict* Logic::getDict(const string& name)
{
    auto name_stem = fs::path(name).stem();
    auto dict =
        find_if(mDicts.begin(), mDicts.end(), [this, name_stem](auto& d) {
            return fs::path(d.getFilename()).stem() == name_stem;
        });
    if (dict != mDicts.end())
        return &(*dict);
    else
        return nullptr;
}
//------------------------------------------------------------------------------
bool Logic::initWithConfig()
{
    return initWithConfig("./config.json");
}
//-----------------------------------------------------------------------------
bool Logic::initWithConfig(const std::string& filename)
{
    bool ret = true;

    try
    {
        loadConfig(filename);
        mFilename = filename;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error during load configuration file: " << e.what()
                  << '\n';
        ret = false;
    }

    try
    {
        refreshAvailableDicts();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error during serching for new dictionaries: " << e.what()
                  << '\n';
        ret = false;
    }
    return ret;
}
void Logic::loadConfig(const std::string& filename)
{
    ifstream cfg_file(filename);

    if (cfg_file)
    {
        cout << "Loading config" << endl;
        json cfg;
        cfg_file >> cfg;

        // load dictionaries
        if (cfg.count("dicts") > 0 && cfg["dicts"].is_array())
        {
            for (json& dict : cfg["dicts"])
            {
                string filename = dict[0];
                if (fs::exists(filename))
                {
                    cout << "importing new dict: " << filename << endl;
                    mDicts.emplace_back(filename, dict[1], dict[2]);
                }
                else
                {
                    // mDicts.emplace_back(filename, dict[1], dict[2]);
                    cerr << "dictionary does not exist: '" << filename << "'"
                         << endl;
                }
            }
        }

        if (cfg.count("position") > 0 && cfg["position"].size() == 2)
        {
            cout << "loading position" << endl;
            mPositionX = cfg["position"][0];
            mPositionY = cfg["position"][1];
        }
        if (cfg.count("size") > 0 && cfg["size"].size() == 2)
        {
            cout << "loading size" << endl;
            mSizeX = cfg["size"][0];
            mSizeY = cfg["size"][1];
        }
        if (cfg.count("translateClipboardAtStart") > 0)
            mTranslateClipboardAtStart = cfg["translateClipboardAtStart"];

        if (cfg.count("alwaysOnTop") > 0)
            mAlwaysOnTop = cfg["alwaysOnTop"];

        if (cfg.count("lastDictForNewWord") > 0)
            mLastDictForNewWord = cfg["lastDictForNewWord"];


        // cout << std::setw(4) << cfg <<endl;
    }
}
//------------------------------------------------------------------------------
void Logic::saveConfig(const std::string& filename)
{
    // prepare json object with settings
    cout << "Saving config" << endl;
    json cfg;
    for (auto& dict : mDicts)
        cfg["dicts"].push_back(
            {dict.getFilename(), dict.mBonus, dict.is_enabled()});

    cfg["size"] = {mSizeX, mSizeY};
    cfg["position"] = {mPositionX, mPositionY};
    cfg["translateClipboardAtStart"] = mTranslateClipboardAtStart;
    cfg["alwaysOnTop"] = mAlwaysOnTop;
    cfg["lastDictForNewWord"] = mLastDictForNewWord;

    // save settings
    ofstream outFile(filename);
    outFile << std::setw(4) << cfg << endl;
}
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifdef UNIT_TESTS
#include "catch.hpp"

TEST_CASE("loading dicts")
{
    Logic l;
    l.mDicts.emplace_back("test.dict");
    l.mDicts.emplace_back("../some_path/test2.dict");
    l.mDicts.emplace_back("../some path/test3.dict");
    REQUIRE(l.getDict("test.dict") != nullptr);
    REQUIRE(l.getDict("testx.dict") == nullptr);
    REQUIRE(l.getDict("test2") != nullptr);
    REQUIRE(l.getDict("test2.dict") != nullptr);
    REQUIRE(l.getDict("test3.dict") != nullptr);
    REQUIRE(l.getDict("../other/path/test3") != nullptr);
}

#endif
