#include "./Logic.h"
#include "json.hpp"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <version.h>

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
    // auto path = fs::current_path() / ".." / "share" / "runningDict";
    auto path = fs::path(getPackagePath()) / "share" / "runningDict";

    this->loadDictsInDir(path.string());
    this->loadDictsInDir(mConfigDir);

    for (auto& dir : mAdditionalSearchDirs)
        this->loadDictsInDir(dir);
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
                // auto relPath = relativeTo(fs::current_path(), file.path());
                auto relPath = fs::canonical(file.path());
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
std::string Logic::getPackagePath()
{
    cout<<"test"<<endl;
#ifdef WIN32
    if (fs::exists("./bin/runningDictGui.exe"))
        return "./";
    if (fs::exists("./runningDictGui.exe"))
        return "../";
    if (fs::exists("c:/Program Files/runningdict"))
        return "c:/Program Files/runningdict/";
    if (fs::exists("c:/Program Files (x86)/runningdict"))
        return "c:/Program Files (x86)/runningdict/";
#else
    if (fs::exists("./bin/runningDictGui"))
        return "./";
    if (fs::exists("./runningDictGui"))
        return "../";
    if (fs::exists("/usr/bin/runningDictGui"))
        return "/usr";
    if (fs::exists("/usr/local/bin/runningDictGui"))
        return "/usr/local/";
    if (fs::exists("/bin/runningDictGui"))
        return "/";
#endif
    return "./";
}
//------------------------------------------------------------------------------
bool Logic::initWithConfig()
{
#ifdef WIN32yy
    fs::path confdir = fs::path{string{getenv("APPDATA")}} / "runningdict";
#else
    fs::path confdir =
        fs::path{string{getenv("HOME")}} / ".config" / "runningdict";
#endif
    if (not fs::exists(confdir))
    {
        create_directories(confdir);
        cout << "INIT: creating config dir at " << confdir << endl;
    }
    if (not fs::exists(confdir / "user.dict"))
    {
        auto userdictpath = confdir / "user.dict";
        std::ofstream outfile(userdictpath.string());
        cout << "INIT: creating empty user dict at " << confdir / "user.dict"
             << endl;
    }
    mConfigDir = fs::absolute(confdir).string();


    auto configPath = confdir / "config.json";
    return initWithConfig(configPath.string());
}
//-----------------------------------------------------------------------------
bool Logic::initWithConfig(const std::string& filename)
{
    bool ret = true;

    try
    {
        loadConfig(filename);
        mConfigFilename = filename;
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
                string dictFile = dict[0];
                if (fs::exists(dictFile))
                {
                    cout << "importing new dict: " << dictFile << endl;
                    mDicts.emplace_back(dictFile, dict[1], dict[2]);
                }
                else
                {
                    // mDicts.emplace_back(dictFile, dict[1], dict[2]);
                    cerr << "dictionary does not exist: '" << dictFile << "'"
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
        if (cfg.count("additionalSearchDirs") > 0 &&
            cfg["additionalSearchDirs"].is_array())
        {
            for (string fil : cfg["additionalSearchDirs"])
                if (fs::exists(fs::path(fil)))
                    mAdditionalSearchDirs.emplace_back(fil);
                else
                    cout << "INIT: Search path " << fil << " does not exists"
                         << endl;
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


    cfg["additionalSearchDirs"] = json::array();
    for (auto& dir : mAdditionalSearchDirs)
        cfg["additionalSearchDirs"].push_back(dir);

    cfg["version"] = {
        Version::getMajor(), Version::getMinor(), Version::getPatch()};


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
