#include "Dict.h"
#include "cpr/cpr.h"
#include "json.hpp"
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
    // create empty string
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
    mIs_open = true;
}
//-----------------------------------------------------------------------------
bool Dict::reload()
{
    if (!is_open() && mFilename == "")
        return false;
    return (open(mFilename));
}
//-----------------------------------------------------------------------------
bool Dict::open(const std::string& filename)
{
    // presume problems, reseted later
    mErrorState = true;

    mIs_open = false;
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
    mIs_open = true;
    mErrorState = false;

    // load metadata
    if (fs::exists(filename + ".meta"))
    {
        ifstream metaFile{filename + ".meta"};
        json meta;
        metaFile >> meta;

        mOnline = meta.value("online", false);
        mReadOnly = meta.value("readOnly", false);
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

    std::ofstream outMeta{mFilename+".meta"};
    outMeta << std::setw(4) << meta << endl;
}
//-----------------------------------------------------------------------------
void Dict::sync(const std::string& serverUrl)
{
    if (not mOnline)
        return;

    auto re = cpr::Get(cpr::Url{serverUrl + "/api/version"});
    json r = json::parse(re.text);
}
//-----------------------------------------------------------------------------
std::shared_ptr<const std::string> Dict::getContens() const
{
    if (not mIs_open)
        throw std::domain_error{
            "Dictionary \"" + mFilename + "\" was not loaded."};
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

bool Dict::addWord(const std::string& word, const std::string& translation)
{
    if (not mIs_open)
        throw std::domain_error{
            "Dictionary \"" + mFilename + "\" was not loaded."};
    // this->open(mFilename);
    if (not mIs_open)
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
bool Dict::hasWord(const std::string& word)
{
    auto holder = mContent;
    std::istringstream iss{*holder};
    for (std::string line; std::getline(iss, line);)
        if (line == word)
            return true;
    return false;
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
    return result;
}
//-----------------------------------------------------------------------------
bool Dict::is_enabled()
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
        if (not mIs_open)
            mEnabled = reload();
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
const std::string& Dict::getFilename() const
{
    return mFilename;
}
//-----------------------------------------------------------------------------
bool Dict::is_open()
{
    return mIs_open;
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

TEST_CASE("Test syncing of dictionary", "[!hide][server]")
{
    Dict d;
    d.setName("testDictionary");
    d.mOnline = true;
    d.fill("ein\n one\nzwei\n zwei\ndrei\n three");

    d.sync("localhost:3000");

    // REQUIRE();
}
#endif
