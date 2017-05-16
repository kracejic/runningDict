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
    L->debug("Logic created");
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
    L->debug("refreshing available dictionaries");
    auto path = fs::path(getPackagePath()) / "share" / "runningDict";

    this->loadDictsInDir(path.string());
    this->loadDictsInDir(mConfigDir);

    for (auto& dir : mAdditionalSearchDirs)
        this->loadDictsInDir(dir);
}
//-----------------------------------------------------------------------------
void Logic::loadDictsInDir(const std::string& path)
{
    L->debug("load directories in {}", path);

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
                L->debug("Found new dict: {}", file.path().string());
                L->debug("      rel path: {}", relPath.string());
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
bool Logic::createDict(const std::string& filename, bool online)
{
    auto confdir = fs::path(getConfigPath());
    auto userdictpath = confdir / (filename + ".dict");
    if (online)
        userdictpath = confdir / "sync" / (filename + ".dict");

    L->info("creating new empty user dict at {}", userdictpath.string());
    if (fs::exists(userdictpath))
    {
        L->warn("... file already exists");
        return false;
    }

    std::unique_lock<std::mutex> lock(dictsLock);
    mDicts.emplace_back();
    mDicts.back().setName(filename);
    mDicts.back().setFileName(userdictpath.string());
    if (online)
        mDicts.back().mOnline = true;
    mDicts.back().saveDictionary();

    return true;
}
//------------------------------------------------------------------------------
bool Logic::initWithConfig()
{
    L->debug("bool Logic::initWithConfig()");
    auto confdir = fs::path(getConfigPath());
    L->debug("config dir is {}", confdir.string());
    if (not fs::exists(confdir))
    {
        L->debug("... directory does not exist, creating");
        create_directories(confdir);
        L->debug("INIT: creating config dir at {}", confdir.string());
    }
    if (not fs::exists(confdir / "user.dict"))
    {
        L->debug("... user.dict does not exist, creating");
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
    L->debug("bool Logic::initWithConfig({})", filename);

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
                    L->debug("importing new dict: {}", dictFile);
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
            L->debug("loading position");
            mPositionX = cfg["position"][0];
            mPositionY = cfg["position"][1];
        }
        if (cfg.count("size") > 0 && cfg["size"].size() == 2)
        {
            L->debug("loading size");
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
                    L->warn("INIT: Search path {} does not exists", fil);
        }

        if (cfg.count("translateClipboardAtStart") > 0)
            mTranslateClipboardAtStart = cfg["translateClipboardAtStart"];

        if (cfg.count("alwaysOnTop") > 0)
            mAlwaysOnTop = cfg["alwaysOnTop"];

        if (cfg.count("lastDictForNewWord") > 0)
            mLastDictForNewWord = cfg["lastDictForNewWord"];

        if (cfg.count("server") > 0)
            mServer = cfg["server"];

        mDebug = cfg.value("debug", false);
        if (mDebug)
            L->set_level(spdlog::level::debug);
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
            {dict.getFilename(), dict.mBonus, dict.isEnabled()});

    cfg["size"] = {mSizeX, mSizeY};
    cfg["position"] = {mPositionX, mPositionY};
    cfg["translateClipboardAtStart"] = mTranslateClipboardAtStart;
    cfg["alwaysOnTop"] = mAlwaysOnTop;
    cfg["lastDictForNewWord"] = mLastDictForNewWord;
    cfg["server"] = mServer;
    cfg["debug"] = mDebug;


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
void Logic::sortDicts()
{
    std::unique_lock<std::mutex> lock(dictsLock);
    sort(mDicts.begin(), mDicts.end(), [](auto& lh, auto& rh) {
        if (lh.isEnabled() && !rh.isEnabled())
            return true;
        if (!lh.isEnabled() && rh.isEnabled())
            return false;
        if (lh.mOnline && !rh.mOnline)
            return true;
        if (!lh.mOnline && rh.mOnline)
            return false;
        return lh.getName() > rh.getName();
    });
}
#include "cpr/cpr.h"
future<void> Logic::connectToServerAndSync()
{
    return connectToServerAndSync(mServer);
}
future<void> Logic::connectToServerAndSyncIfItIsNeccessary()
{
    bool shouldSync = false;
    if ((std::chrono::system_clock::now() - mLastServerSync) > 10min)
        shouldSync = true;

    for (auto& dict : mDicts)
        if (dict.isDirty())
            shouldSync = true;

    if (shouldSync)
        return connectToServerAndSync();
    else
        return {};
}

future<void> Logic::connectToServerAndSync(const std::string& url)
{
    if (mServerStatus == ServerStatus::offline)
        mServerStatus = ServerStatus::connecting;
    mLastServerSync = std::chrono::system_clock::now();
    L->info("Trying to sync with server: {}", url);

    auto fut = async(launch::async, [this, url]() {
        if (url == "")
        {
            L->warn("Server url is emtpy");
            return;
        }

        // test if server is there
        auto re = cpr::Get(cpr::Url{url + "/api/version"}, cpr::Timeout{2000});
        if (re.status_code != 200)
        {
            L->info("Server not available: returns {}", re.status_code);
            mServerStatus = ServerStatus::serverNotAvailable;
            return;
        }
        json r = json::parse(re.text);

        string expectedversion = "0.1.";
        // check server type and compatible versions
        if (r["app"] == "dictionaryServer" &&
            strncmp(expectedversion.c_str(), expectedversion.c_str(),
                expectedversion.size()) == 0)
        {
            this->mServerStatus = ServerStatus::connected;
        }
        else
        {
            mServerStatus = ServerStatus::serverError;
            L->info("Server connection was not succesfull.");
            return;
        }

        // sync dictionaries
        mServerStatus = ServerStatus::synchronizing;
        for (auto&& dict : mDicts)
            if (dict.isLoaded() || dict.isEnabled())
                dict.sync(url);

        // download new dictionaries
        re = cpr::Get(cpr::Url{url + "/api/dictionary"});
        if (re.status_code != 200)
        {
            mServerStatus = ServerStatus::serverError;
            L->warn("Error {} dictionary: {}", re.status_code, re.text);
            return;
        }
        L->debug("List of dictionaries on server: {}", re.text);
        json dictsFromServer = json::parse(re.text);

        // recreate sync folder
        fs::path syncDirPath = fs::path(mConfigDir) / "sync";
        if (not fs::exists(syncDirPath))
            fs::create_directories(syncDirPath);

        // setup new dictionaries
        if (dictsFromServer.size() > 0)
        {
            std::unique_lock<std::mutex> lock(dictsLock);
            for (auto& obj : dictsFromServer)
            {
                string name = obj["name"];
                if (any_of(mDicts.begin(), mDicts.end(),
                        [&name](auto& it) { return it.getName() == name; }))
                    continue;
                L->info("New dictionary found: {}", name);

                this->mDicts.emplace_back();
                mDicts.back().setName(name);
                mDicts.back().enable(false);
                mDicts.back().mOnline = true;
                mDicts.back().setFileName(
                    (syncDirPath / name += ".dict").string());
                // no syncing yet, syncing, when user enables it
            }
        }


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
    auto lockedDicts = l.getDicts();

    lockedDicts.dicts.emplace_back("test.dict");
    lockedDicts.dicts.emplace_back("../some_path/test2.dict");
    lockedDicts.dicts.emplace_back("../some path/test3.dict");
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
