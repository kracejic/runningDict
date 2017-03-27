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

#include "log.h"

Logic::Logic()
{
    logging::init(getConfigPath());
    L->info("Logic created");
}

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
    L->info("refreshing available dictionaries");
    auto path = fs::path(getPackagePath()) / "share" / "runningDict";

    this->loadDictsInDir(path.string());
    this->loadDictsInDir(mConfigDir);

    for (auto& dir : mAdditionalSearchDirs)
        this->loadDictsInDir(dir);
}
//-----------------------------------------------------------------------------
void Logic::loadDictsInDir(const std::string& path)
{
    L->info("load directories in {}", path);

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
                mDicts.emplace_back(relPath.string(), 0, true);
                L->info("Found new dict: {}", file.path().string());
                L->info("      rel path: {}", relPath.string());
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
//-----------------------------------------------------------------------------
std::string Logic::getPackagePath()
{
#ifdef WIN32
    if (fs::exists("bin/runningDictGui.exe"))
        return ".";
    if (fs::exists("runningDictGui.exe"))
        return "..";
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
    return "";
}
//------------------------------------------------------------------------------
std::string Logic::getConfigPath()
{
#ifdef WIN32
    auto confdir = fs::path{string{getenv("APPDATA")}} / "runningdict";
#else
    auto confdir = fs::path{string{getenv("HOME")}} / ".config" / "runningdict";
#endif
    return confdir.string();
}
//-----------------------------------------------------------------------------
bool Logic::createDict(const std::string& filename)
{
    auto confdir = fs::path(getConfigPath());
    auto userdictpath = confdir / (filename + ".dict");
    L->info("creating new empty user dict at {}", userdictpath.string());
    if (fs::exists(userdictpath))
    {
        L->warn("... file already exists");
        return false;
    }
    std::ofstream outfile(userdictpath.string());
    outfile.close();
    this->refreshAvailableDicts();
    return true;
}
//------------------------------------------------------------------------------
bool Logic::initWithConfig()
{
    L->info("bool Logic::initWithConfig()");
    auto confdir = fs::path(getConfigPath());
    L->info("config dir is {}", confdir.string());
    if (not fs::exists(confdir))
    {
        L->info("... directory does not exist, creating");
        create_directories(confdir);
        L->info("INIT: creating config dir at {}", confdir.string());
    }
    if (not fs::exists(confdir / "user.dict"))
    {
        L->info("... user.dict does not exist, creating");
        auto userdictpath = confdir / "user.dict";
        std::ofstream outfile(userdictpath.string());
    }
    mConfigDir = fs::absolute(confdir).string();


    auto configPath = confdir / "config.json";
    return initWithConfig(configPath.string());
}
//-----------------------------------------------------------------------------
bool Logic::initWithConfig(const std::string& filename)
{
    bool ret = true;
    L->info("bool Logic::initWithConfig({})", filename);

    try
    {
        loadConfig(filename);
        mConfigFilename = filename;
    }
    catch (const std::exception& e)
    {
        L->error("Error during load configuration file: {}", e.what());
        ret = false;
    }

    try
    {
        refreshAvailableDicts();
    }
    catch (const std::exception& e)
    {
        L->error("Error during serching for new dictionaries: {}", e.what());
        ret = false;
    }
    return ret;
}
void Logic::loadConfig(const std::string& filename)
{
    ifstream cfg_file(filename);

    if (cfg_file)
    {
        L->info("Loading config");
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
                    L->info("importing new dict: {}", dictFile);
                    mDicts.emplace_back(dictFile, dict[1], dict[2]);
                }
                else
                {
                    L->error("dictionary does not exist: {}", dictFile);
                }
            }
        }

        if (cfg.count("position") > 0 && cfg["position"].size() == 2)
        {
            L->info("loading position");
            mPositionX = cfg["position"][0];
            mPositionY = cfg["position"][1];
        }
        if (cfg.count("size") > 0 && cfg["size"].size() == 2)
        {
            L->info("loading size");
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
                    L->info("INIT: Search path {} does not exists", fil);
        }

        if (cfg.count("translateClipboardAtStart") > 0)
            mTranslateClipboardAtStart = cfg["translateClipboardAtStart"];

        if (cfg.count("alwaysOnTop") > 0)
            mAlwaysOnTop = cfg["alwaysOnTop"];

        if (cfg.count("lastDictForNewWord") > 0)
            mLastDictForNewWord = cfg["lastDictForNewWord"];

        if (cfg.count("server") > 0)
            mServer = cfg["server"];
    }
}
//------------------------------------------------------------------------------
void Logic::saveConfig(const std::string& filename)
{
    // prepare json object with settings
    L->info("Saving config");
    json cfg;
    for (auto& dict : mDicts)
        cfg["dicts"].push_back(
            {dict.getFilename(), dict.mBonus, dict.is_enabled()});

    cfg["size"] = {mSizeX, mSizeY};
    cfg["position"] = {mPositionX, mPositionY};
    cfg["translateClipboardAtStart"] = mTranslateClipboardAtStart;
    cfg["alwaysOnTop"] = mAlwaysOnTop;
    cfg["lastDictForNewWord"] = mLastDictForNewWord;
    cfg["server"] = mServer;


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
#include "cpr/cpr.h"
future<void> Logic::connectToServerAndSync()
{
    return connectToServerAndSync(mServer);
}

future<void> Logic::connectToServerAndSync(const std::string& url)
{
    if (mServerStatus == ServerStatus::offline)
        mServerStatus = ServerStatus::connecting;
    mLastServerSync = std::chrono::system_clock::now();
    L->info("Trying to sync with server: {}", url);

    auto fut = async(launch::async, [this, url]() {
        if (url == "")
            return;

        // test if server is there
        auto re = cpr::Get(cpr::Url{url + "/api/version"});
        if (re.status_code != 200)
        {
            mServerStatus = ServerStatus::serverNotAvailable;
            return;
        }
        json r = json::parse(re.text);
        L->info(re.text);
        L->info(r.dump());
        L->info(r["app"].get<string>());
        L->info(r["version"].get<string>());

        // check server type and compatible versions
        if (r["app"] == "dictionaryServer" &&
            r["version"].get<string>()[0] == '0')
            this->mServerStatus = ServerStatus::connected;
        else
            return;
        L->info("Server connection succesfull ({})", url);

        // sync dictionaries
        mServerStatus = ServerStatus::synchronizing;
        for (auto&& dict : mDicts)
            dict.sync(url);

        mLastServerSync = std::chrono::system_clock::now();
        mServerStatus = ServerStatus::connected;
    });
    return fut;
}


//-----------------------------------------------------------------------------
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

TEST_CASE("Connect to server", "[!hide][server]")
{
    Logic l;
    auto fut = l.connectToServerAndSync("localhost:3000");
    fut.get();
    REQUIRE(l.mServerStatus == ServerStatus::connected);
}

#endif
