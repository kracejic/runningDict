#include "Dict.h"
#include "cpr/cpr.h"
#include "json.hpp"
#include "log.h"
#include <algorithm>
#include <iostream>
#include <sstream>

#ifdef USE_BOOST_FILESYSTEM
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

using namespace std;
using json = nlohmann::json;

Dict::Dict()
{
    mContent.reset(new std::string(""));
}
//-----------------------------------------------------------------------------
Dict::Dict(const std::string& filename, int bonus, bool enabled)
    : mEnabled(enabled)
    , mBonus(bonus)
{
    mFilename = filename;
    mName = fs::path(filename).stem().string();
    mContent.reset(new std::string(""));
    this->reload();
}
//-----------------------------------------------------------------------------
void Dict::fill(const std::string& content)
{
    string temp = content;
    temp.erase(std::remove(temp.begin(), temp.end(), '\r'), temp.end());
    mContent.reset(new string(std::move(temp)));
    mIsLoaded = true;
}
//-----------------------------------------------------------------------------
bool Dict::reload()
{
    if (not mIsLoaded && mFilename == "")
        return false;
    return (open(mFilename));
}
//-----------------------------------------------------------------------------
bool Dict::open(const std::string& filename)
{
    mIsLoaded = false;
    string tmp;

    // reopen
    std::ifstream file;
    file.open(filename);

    if (!file.is_open())
        return false;
    mFilename = filename;

    file.seekg(0, std::ios::end);
    tmp.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    tmp.assign((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    file.close();
    tmp.erase(std::remove(tmp.begin(), tmp.end(), '\r'), tmp.end());
    mContent.reset(new std::string(move(tmp)));
    mIsLoaded = true;
    mErrorState = false;

    // load metadata
    if (fs::exists(filename + ".meta"))
    {
        ifstream metaFile{filename + ".meta"};
        json meta;
        metaFile >> meta;

        mOnline = meta.value("online", false);
        mReadOnly = meta.value("readOnly", false);
        if (mOnline)
            mIsSynchronized = true;
    }

    return true;
}
//-----------------------------------------------------------------------------
void Dict::saveDictionary()
{
    auto holder = mContent;
    if (mFilename == "")
        return;
    std::ofstream outfile{mFilename};
    outfile << *holder << endl;

    // save metadata
    json meta;
    meta["online"] = mOnline;
    meta["readOnly"] = mReadOnly;

    std::ofstream outMeta{mFilename + ".meta"};
    outMeta << std::setw(4) << meta << endl;
}
//-----------------------------------------------------------------------------
bool Dict::sync(const std::string& serverUrl)
{
    if (not mOnline)
        return false;
    L->info("Synchronizing dict {} with {}", mName, serverUrl);

    auto re = cpr::Get(cpr::Url{serverUrl + "/api/dictionary"});
    if (re.status_code != 200)
    {
        L->warn("Getting list of dictionaries failed with {}, {}", re.status_code, re.text);
        return false;
    }

    json r = json::parse(re.text);
    for (auto& dict : r)
    {
        if (mName == dict["name"].get<string>())
        {
            L->debug("Dictionary found on server, synchronizing");
            if (*mContent == "")
            {
                L->debug("Dictionary is empty, downloading");
                auto re2 =
                    cpr::Get(cpr::Url{serverUrl + "/api/dictionary/" + mName},
                        cpr::Parameters{{"dict", mName}});
                if (re2.status_code != 200)
                {
                    L->warn("Fetching changes failed with {}, {}", re.status_code, re.text);
                    return false;
                }
                json r2 = json::parse(re2.text);
                fill(r2["text"]);
                revision = r2["revision"];
            }
            else
            {
                L->debug("Dictionary is not empty, synchronizing history");
                if (not this->synchronizeHistory(serverUrl))
                    return false;
            }
            saveDictionary();
            mIsSynchronized = true;
            mIsLoaded = true;
            mErrorState = false;
            return true;
        }
    }

    // This dictionary is not on server
    L->debug("Uploading to server");
    // when dictionary is not on server, it is created and filled with current
    // data.

    /// @todo fix
    auto re3 = cpr::Post(cpr::Url{serverUrl + "/api/dictionary"},
        cpr::Payload{{"name", mName}, {"text", *mContent}});
    L->debug("What we are uploading: {}", *mContent);
    L->debug("re3.text = {}", re3.text);
    L->debug("re3.status_code = {}", re3.status_code);

    if (re3.status_code != 201)
        return false;

    json r3 = json::parse(re3.text);
    revision = r3["revision"];

    saveDictionary();
    mIsSynchronized = true;
    mIsLoaded = true;
    mErrorState = false;
    return true;
}
//-----------------------------------------------------------------------------
bool Dict::synchronizeHistory(const std::string& serverUrl)
{
    // Prepare changelist in json array
    json bundle;
    bundle["changes"] = json::array();
    for (const auto& change : history)
    {
        switch (change.changeType)
        {
            case ChangeType::addWord:
                bundle["changes"].push_back(
                    {{"type", "add"}, {"word", change.word},
                        {"translation", change.translation}});
                break;
            case ChangeType::deleteWord:
                bundle["changes"].push_back(
                    {{"type", "delete"}, {"word", change.word}});
                break;
            case ChangeType::changeWord:
                bundle["changes"].push_back({{"type", "change"},
                    {"word", change.word}, {"translation", change.translation},
                    {"newWord", change.wordNew}});
                break;
        }
    }
    bundle["dict"] = mName;
    bundle["revision"] = revision;

    L->debug("Our request: {}", bundle.dump());

    // Sync with server (push our changes, receive server changes)
    auto re = cpr::Post(cpr::Url{serverUrl + "/api/sync/dictionary/" + mName},
        cpr::Body{bundle.dump()},
        cpr::Header{{"content-type", "application/json"}});

    L->debug("Response: {}", re.text);
    if (re.status_code != 200)
    {
        L->warn("Sychronizing dict failed with {}, {}", re.status_code, re.text);
        return false;
    }

    json response = json::parse(re.text);
    for (const auto& change : response["changes"])
    {
        if (change["type"] == "add")
            this->_addWord(change["word"], change["translation"]);
        if (change["type"] == "delete")
            this->_deleteWord(change["word"]);
        if (change["type"] == "change")
            this->_changeWord(change["word"], change["translation"],
                change["newTranslation"]);
    }
    revision = response["revision"];
    history.clear();

    return true;
}
//-----------------------------------------------------------------------------
future<bool> Dict::deleteFromServer(const std::string& serverUrl)
{
    return async(std::launch::async, [this, serverUrl]() {
        if (not mOnline)
            return false;
        auto re = cpr::Delete(cpr::Url{serverUrl + "/api/dictionary/" + mName},
            cpr::Parameters{{"dict", mName}});
        if (re.status_code != 200)
            L->debug("Delete from server for {} not succesfull: \n   {}", mName,
                re.text);
        return re.status_code == 200;
    });
}
//-----------------------------------------------------------------------------
std::shared_ptr<const std::string> Dict::getContens() const
{
    // if (not mIsLoaded)
    //     throw std::domain_error{
    //         "Dictionary \"" + mFilename + "\" was not loaded."};
    return mContent;
}
//-----------------------------------------------------------------------------
long long Dict::getContensSize() const
{
    return mContent->size();
}
//-----------------------------------------------------------------------------
string getLowerCase2(const string& txt)
{
    string res{txt};
    for (auto&& i : res)
    {
        if ((unsigned char)i < 128)
            i = tolower(i);
    }
    return res;
}
//-----------------------------------------------------------------------------
bool Dict::hasWord(const std::string& word) const
{
    auto holder = mContent;
    std::istringstream iss{*holder};
    for (std::string line; std::getline(iss, line);)
        if (line == word)
            return true;
    return false;
}
//-----------------------------------------------------------------------------
std::string Dict::translationOfWord(const std::string& word) const
{
    auto holder = mContent;
    std::istringstream iss{*holder};
    for (std::string line; std::getline(iss, line);)
        if (line == word)
        {
            std::getline(iss, line);
            return line;
        }
    return "";
}
//-----------------------------------------------------------------------------
bool Dict::operator==(const Dict& d) const
{

    auto holder = mContent;
    std::istringstream iss{*holder};
    for (std::string line; std::getline(iss, line);)
    {
        if (line[0] == ' ')
            continue;
        string translation;
        getline(iss, translation);

        if (not d.hasWord(line))
            return false;
        if (d.translationOfWord(line) != translation)
            return false;
    }

    return true;
}
bool Dict::operator!=(const Dict& d) const
{
    return not this->operator==(d);
}
//-----------------------------------------------------------------------------
bool Dict::addWord(const std::string& word, const std::string& translation)
{
    if (_addWord(word, translation))
    {
        history.emplace_back(ChangeType::addWord, word, translation);
        return true;
    }
    return false;
}
bool Dict::_addWord(const std::string& word, const std::string& translation)
{
    if (not mIsLoaded)
        throw std::domain_error{
            "Dictionary \"" + mFilename + "\" was not loaded."};
    // this->open(mFilename);
    if (not mIsLoaded)
        return false;

    // Make lower case
    string wordCopy = getLowerCase2(word);

    // erase whitespace on the end of mContent
    string tmp = *mContent;
    tmp.erase(std::find_if(tmp.rbegin(), tmp.rend(),
                  std::not1(std::ptr_fun<int, int>(std::isspace)))
                  .base(),
        tmp.end());

    // add word
    tmp.append("\n"s + wordCopy + "\n " + translation);
    if (tmp[0] == '\n')
        tmp.erase(0, 1);

    mContent.reset(new string(move(tmp)));

    this->saveDictionary();

    return true;
}
//-----------------------------------------------------------------------------
bool myIsPunct2(char ch)
{
    switch (ch)
    {
        case '!':
        case '"':
        case '#':
        case '$':
        case '%':
        case '&':
        case '\'':
        case '(':
        case ')':
        case '*':
        case '+':
        case ',':
        case '-':
        case '.':
        case '/':
        case ':':
        case ';':
        case '<':
        case '=':
        case '>':
        case '?':
        case '@':
        case '[':
        case '\\':
        case ']':
        case '^':
        case '_':
        case '`':
        case '{':
        case '|':
        case '}':
        case '~': return true;
        default: return false;
    }
}
bool compare_weak(const std::string& lhs, const std::string& rhs)
{
    string longer = lhs;
    string shorter = rhs;
    if (lhs.size() < rhs.size())
    {
        longer = rhs;
        shorter = lhs;
    }

    for (size_t i = 0; i < longer.size(); ++i)
    {
        if (i < shorter.size())
        {
            if (myIsPunct2(shorter[i]) || myIsPunct2(longer[i]))
                return true;
            if (lhs[i] != rhs[i])
                return false;
        }
        else
        {
            if (myIsPunct2(longer[i]))
                return true;
            if (longer[i] != ' ')
                return false;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
bool Dict::changeWord(const std::string& word,
    const std::string& newTranslation, const std::string& wordNew)
{
    if (_changeWord(word, newTranslation, wordNew))
    {
        history.emplace_back(
            ChangeType::changeWord, word, newTranslation, wordNew);
        return true;
    }
    return false;
}
bool Dict::_changeWord(const std::string& word,
    const std::string& newTranslation, const std::string& wordNew)
{
    bool result = false;
    auto holder = mContent;
    std::istringstream iss{*holder};
    string output;
    string translation = newTranslation;
    std::replace(translation.begin(), translation.end(), '\n', ';');
    for (std::string line; std::getline(iss, line);)
    {
        // todo compare to the first non asci character
        if (compare_weak(line, word))
        {
            result = true;
            if (wordNew == "")
                output += line + "\n";
            else
                output += wordNew + "\n";
            output += " " + translation + "\n";
            while (true)
            {

                if (std::getline(iss, line))
                    break;
                if (line[0] != ' ')
                {
                    output += line + "\n";
                    break;
                }
            }
        }
        else
            output += line + "\n";
    }
    output.erase(std::find_if(output.rbegin(), output.rend(),
                     std::not1(std::ptr_fun<int, int>(std::isspace)))
                     .base(),
        output.end());
    mContent.reset(new std::string(output));
    this->saveDictionary();

    return result;
}
//-----------------------------------------------------------------------------
bool Dict::deleteWord(const std::string& word)
{
    if (_deleteWord(word))
    {
        history.emplace_back(ChangeType::deleteWord, word);
        return true;
    }
    return false;
}
bool Dict::_deleteWord(const std::string& word)
{
    bool result = false;
    auto holder = mContent;
    std::istringstream iss{*holder};
    string output;
    for (std::string line; std::getline(iss, line);)
    {
        // todo compare to the first non asci character
        if (compare_weak(line, word))
        {
            result = true;
            while (true)
            {
                if (std::getline(iss, line))
                    break;
                if (line[0] != ' ')
                {
                    output += line + "\n";
                    break;
                }
            }
        }
        else
            output += line + "\n";
    }
    output.erase(std::find_if(output.rbegin(), output.rend(),
                     std::not1(std::ptr_fun<int, int>(std::isspace)))
                     .base(),
        output.end());
    mContent.reset(new std::string(output));
    this->saveDictionary();

    // Make it recursive
    if (result)
        _deleteWord(word);
    return result;
}
//-----------------------------------------------------------------------------
bool Dict::isEnabled()
{
    return mEnabled;
}
//-----------------------------------------------------------------------------
bool Dict::toogle_enable()
{
    return this->enable(not mEnabled);
}
//-----------------------------------------------------------------------------
bool Dict::enable(bool state)
{
    if (state)
    {
        mEnabled = true;
        if (not mIsLoaded)
            mEnabled = reload() || mOnline;
    }
    else
        mEnabled = false;

    return mEnabled;
}
//-----------------------------------------------------------------------------
void Dict::setName(const std::string& name)
{
    mName = name;
}
//-----------------------------------------------------------------------------
const std::string& Dict::getName() const
{
    return mName;
}
//-----------------------------------------------------------------------------
void Dict::setFileName(const std::string& name)
{
    mFilename = name;
}
//-----------------------------------------------------------------------------
const std::string& Dict::getFilename() const
{
    return mFilename;
}
//-----------------------------------------------------------------------------


#ifdef UNIT_TESTS
#include "catch.hpp"
TEST_CASE("adding a word")
{
    Dict d;
    d.fill("ein\n one\nzwei\n zwei");
    d.addWord("drei", "three");
    REQUIRE(*(d.getContens()) == "ein\n one\nzwei\n zwei\ndrei\n three");
    d.addWord("vier", "four");
    REQUIRE(*(d.getContens()) ==
            "ein\n one\nzwei\n zwei\ndrei\n three\nvier\n four");
}
TEST_CASE("changing a words in dictionary")
{
    Dict d;
    d.fill("ein\n one\nzwei\n zwei\ndrei\n three");
    REQUIRE(d.hasWord("ein"));
    REQUIRE(d.hasWord("zwei"));
    REQUIRE(d.hasWord("drei"));
    REQUIRE(not d.hasWord("drei "));
    REQUIRE(not d.hasWord("one"));
    REQUIRE(not d.hasWord("three"));
    REQUIRE(not d.hasWord("one"));
    REQUIRE(not d.hasWord("one"));
}
TEST_CASE("add a word to the empty dictionary")
{
    Dict d{"test2.dict"};
    REQUIRE(*(d.getContens()) == "katze\n cat\nhund\n dog\n");
    d.addWord("drei", "three");
    REQUIRE(*(d.getContens()) == "katze\n cat\nhund\n dog\ndrei\n three");
    REQUIRE(d.changeWord("drei", "tri") == true);
    REQUIRE(*(d.getContens()) == "katze\n cat\nhund\n dog\ndrei\n tri");
    d.addWord("aa", "bb");
    REQUIRE(
        *(d.getContens()) == "katze\n cat\nhund\n dog\ndrei\n tri\naa\n bb");
    REQUIRE(d.deleteWord("drei") == true);
    REQUIRE(*(d.getContens()) == "katze\n cat\nhund\n dog\naa\n bb");
    REQUIRE(d.deleteWord("aa") == true);
    REQUIRE(d.deleteWord("aa") == false);
    REQUIRE(*(d.getContens()) == "katze\n cat\nhund\n dog");
}

TEST_CASE("compare_weak")
{
    REQUIRE(compare_weak("jedna", "jedna"));
    REQUIRE(compare_weak("jedna dva", "jedna dva"));
    REQUIRE(compare_weak("jedna ", "jedna"));
    REQUIRE(compare_weak("jedna", "jedna "));
    REQUIRE(compare_weak("jedna", "jedna "));
    REQUIRE(not compare_weak("jedna", "jedna dva"));
    REQUIRE(not compare_weak("jedna", "jedna dva /asdad/"));
    REQUIRE(compare_weak("jedna", "jedna/test/ "));
    REQUIRE(compare_weak("jedna", "jedna /test/ "));
    REQUIRE(compare_weak("jedna dva", "jedna dva/test/ "));
    REQUIRE(compare_weak("jedna dva", "jedna dva /test/ "));
    REQUIRE(compare_weak("jedna dva ", "jedna dva /test/ "));
    REQUIRE(not compare_weak("jedna", "dva"));
    REQUIRE(not compare_weak("jedna ", "dva "));
    REQUIRE(not compare_weak("jedna /", "dva /"));
    REQUIRE(not compare_weak("jedna", "dva /"));
    REQUIRE(not compare_weak("jedna /", "dva"));
}

TEST_CASE("adding to an empty dictionary")
{
    Dict d;
    d.fill("");
    d.addWord("test", "test");
    REQUIRE(*(d.getContens()) == "test\n test");

    d.fill("\n");
    d.addWord("test", "test");
    REQUIRE(*(d.getContens()) == "test\n test");
}

TEST_CASE("cr lf removal")
{
    Dict d;
    d.fill("test\r\n test\n");
    REQUIRE(*(d.getContens()) == "test\n test\n");
}

TEST_CASE("checking for a word")
{
    Dict d;
    d.fill("ein\n one\nzwei\n zwei\ndrei\n three");
    d.changeWord("ein", "jedna");
    REQUIRE(*(d.getContens()) == "ein\n jedna\nzwei\n zwei\ndrei\n three");
    d.changeWord("zwei", "dva");
    REQUIRE(*(d.getContens()) == "ein\n jedna\nzwei\n dva\ndrei\n three");
    d.changeWord("ein", "jedno jednicka, jedna");
    REQUIRE(*(d.getContens()) ==
            "ein\n jedno jednicka, jedna\nzwei\n dva\ndrei\n three");
    d.changeWord("drei", "tricet stribrnych kurat\ntricet stribrnych strech");
    REQUIRE(*(d.getContens()) ==
            "ein\n jedno jednicka, jedna\nzwei\n dva\ndrei\n "
            "tricet stribrnych kurat;tricet stribrnych "
            "strech");

    // tests with special characters
    d.fill("ein /test/\n one\nzwei\n zwei\ndrei\n three");
    d.changeWord("ein", "jedna", "ein");
    REQUIRE(*(d.getContens()) == "ein\n jedna\nzwei\n zwei\ndrei\n three");
    d.changeWord("ein", "jeden", "eine");
    REQUIRE(*(d.getContens()) == "eine\n jeden\nzwei\n zwei\ndrei\n three");
    REQUIRE(d.changeWord("einaaaaa", "jeden", "eine") == false);
}

TEST_CASE("comparison of dictionaries")
{
    Dict d1;
    d1.fill("ein\n one\nzwei\n zwei\ndrei\n three");
    Dict d2;
    d2.fill("ein\n one\nzwei\n zwei\ndrei\n three");
    REQUIRE(d1 == d2);

    Dict d3;
    d3.fill("zwei\n zwei\nein\n one\ndrei\n three");
    REQUIRE(d3 == d2);

    Dict d4;
    d4.fill("drei\n three\nzwei\n zwei\nein\n one\n");
    REQUIRE(d4 == d2);

    Dict d5;
    d5.fill("drei\n three\nzwei\n zweiaa\nein\n one\n");
    REQUIRE(d5 != d2);

    Dict d6;
    d6.fill("drei\n three\nzweiaa\n zweiaa\nein\n one\n");
    REQUIRE(d6 != d2);
}

//-----------------------------------------------------------------------------
// SERVER tests
string server = "localhost:3000";

TEST_CASE("syncing of dictionary with server", "[!hide][server]")
{
    Dict d;
    d.setName("testDictionary");
    d.mOnline = true;
    d.fill("ein\n one\nzwei\n zwei\ndrei\n three");
    d.deleteFromServer(server).get();
    REQUIRE(d.sync(server));

    // test that server has the same dict
    Dict d2;
    d2.mOnline = true;
    d2.setName("testDictionary");
    REQUIRE(d2.sync(server));
    REQUIRE(*(d.getContens()) == *(d2.getContens()));
}

TEST_CASE("adding to server", "[!hide][server]")
{
    Dict d;
    d.setName("testDictionary");
    d.mOnline = true;
    d.fill("ein\n one\nzwei\n zwei\ndrei\n three");
    d.deleteFromServer(server).get();
    REQUIRE(d.sync(server));

    d.addWord("katze", "kocicka");
    REQUIRE(d.sync(server));


    Dict d2;
    d2.mOnline = true;
    d2.setName("testDictionary");
    REQUIRE(d2.sync(server));
    REQUIRE(*(d.getContens()) == *(d2.getContens()));
}

TEST_CASE("change at server", "[!hide][server]")
{
   Dict d;
   d.setName("testDictionary");
   d.mOnline = true;
   d.fill("ein\n one\nzwei\n two\ndrei\n three\nvier\n four\nfünf\n five\nsechs\n six");
   d.deleteFromServer(server).get();
   REQUIRE(d.sync(server));

   d.changeWord("ein", "jeden");
   d.changeWord("drei", "tricet", "dreizig");
   REQUIRE(d.sync(server));

   Dict d2;
   d2.mOnline = true;
   d2.setName("testDictionary");
   REQUIRE(d2.sync(server));
   REQUIRE(d == d2);
}

TEST_CASE("delete from server", "[!hide][server]")
{
   Dict d;
   d.setName("testDictionary");
   d.mOnline = true;
   d.fill("ein\n one\nzwei\n two\ndrei\n three\nvier\n four\nfünf\n five\nsechs\n six");
   d.deleteFromServer(server).get();
   REQUIRE(d.sync(server));

   d.deleteWord("ein");
   d.deleteWord("fünf");
   REQUIRE(d.sync(server));


   Dict d2;
   d2.mOnline = true;
   d2.setName("testDictionary");
   REQUIRE(d2.sync(server));
   REQUIRE(d == d2);
}

TEST_CASE("two dicts no conflict", "[!hide][server]")
{
    Dict d1;
    d1.setName("testDictionary");
    d1.mOnline = true;
    d1.fill("ein\n one\nzwei\n two\ndrei\n three");
    d1.deleteFromServer(server).get();
    REQUIRE(d1.sync(server));

    d1.addWord("katze", "kocicka");
    //d1.changeWord("katze", "kocka");
    //d1.changeWord("drei", "30", "dreizig");
    d1.addWord("hund", "pejsanek");

    Dict d2;
    d2.mOnline = true;
    d2.setName("testDictionary");
    REQUIRE(d2.sync(server));

    d2.addWord("schaf", "ovecka");
    d2.deleteWord("ein");
    d2.addWord("truthahn", "krocan");
    d2.deleteWord("schaf");

    REQUIRE(d1.sync(server));
    REQUIRE(d2.sync(server));
    REQUIRE(d1.sync(server));

    L->info("d1: {}", *d1.getContens());
    L->info("d2: {}", *d2.getContens());

    REQUIRE(d2 == d1);
    REQUIRE(d1 == d2);
    REQUIRE(d1.getRevision() == d2.getRevision());
}


TEST_CASE("two dicts", "[!hide][server]")
{
    Dict d1;
    d1.setName("testDictionary");
    d1.mOnline = true;
    d1.fill("base\n base2");
    d1.deleteFromServer(server).get();
    REQUIRE(d1.sync(server));

    d1.addWord("katze", "kocicka");
    d1.deleteWord("katze");

    Dict d2;
    d2.mOnline = true;
    d2.setName("testDictionary");
    REQUIRE(d2.sync(server));

    d2.addWord("katze", "kocicka2");

    REQUIRE(d1.sync(server));
    REQUIRE(d2.sync(server));
    REQUIRE(d1.sync(server));
    REQUIRE(d2.sync(server));

    L->info("d1: {}", *d1.getContens());
    L->info("d2: {}", *d2.getContens());

    REQUIRE(d2 == d1);
    REQUIRE(d1 == d2);
    REQUIRE(d1.getRevision() == d2.getRevision());
}

// disabled more complex test
TEST_CASE("mulitiple clients", "[server]")
{
    Dict d1;
    d1.setName("testDictionary");
    d1.mOnline = true;
    d1.fill("ein\n one\nzwei\n zwei\ndrei\n three");
    d1.deleteFromServer(server).get();
    REQUIRE(d1.sync(server));

    d1.addWord("katze", "kocicka");
    d1.deleteWord("katze");
    d1.addWord("german", "nemecky");


    Dict d2;
    d2.mOnline = true;
    d2.setName("testDictionary");
    REQUIRE(d2.sync(server));

    d2.addWord("katze", "kocicka2");
    d2.addWord("test", "test2");

    REQUIRE(d1.sync(server));
    REQUIRE(d1.getRevision() == 4);

    REQUIRE(d2.sync(server));
    REQUIRE(d2.getRevision() == 5);

    REQUIRE(d1.sync(server));
    REQUIRE(d1.getRevision() == 5);


    Dict dcheck;
    dcheck.mOnline = true;
    dcheck.setName("testDictionary");
    REQUIRE(dcheck.sync(server));
    L->info("d1: {}", *d1.getContens());
    L->info("d2: {}", *d2.getContens());
    L->info("dcheck: {}", *dcheck.getContens());

    REQUIRE(d2 == d1);
    REQUIRE(d1 == dcheck);
    REQUIRE(d2 == dcheck);
    REQUIRE(dcheck == d1);
    REQUIRE(dcheck == d2);
    REQUIRE(d1.getRevision() == d2.getRevision());
    REQUIRE(dcheck.getRevision() == d1.getRevision());
}
#endif
