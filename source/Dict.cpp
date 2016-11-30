#include "Dict.h"
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace std;

Dict::Dict()
{
}
Dict::Dict(string filename)
{
    mFilename = filename;
}
//------------------------------------------------------------------------------
Dict::Dict(std::string filename, int bonus, bool enabled)
    : mFilename(filename)
    , mEnabled(enabled)
    , mBonus(bonus)
{
    // std::cout<<"New Dict: filename = "<<filename<<std::endl;
    // std::cout<<"  bonus = "<<bonus<<std::endl;
    // std::cout<<"  enabled = "<<enabled<<std::endl;
    if (mEnabled)
        this->reload();
}
//-----------------------------------------------------------------------------------
void Dict::fill(std::string content)
{
    mContent.reset(new std::string(content));
    mIs_open = true;
}
//-----------------------------------------------------------------------------------
const std::string& Dict::getFilename() const
{
    return mFilename;
}
//-----------------------------------------------------------------------------------
bool Dict::reload()
{
    if (!is_open() && mFilename == "")
        return false;
    return (open(mFilename));
}
//-----------------------------------------------------------------------------------
bool Dict::is_enabled()
{
    return mEnabled;
}
//------------------------------------------------------------------------------
bool Dict::toogle_enable()
{
    return this->enable(not mEnabled);
}
//------------------------------------------------------------------------------
bool Dict::enable(bool state)
{
    if (state == true)
    {
        mEnabled = true;
        if (mIs_open == false)
            mEnabled = reload();
    }
    else
        mEnabled = false;

    return mEnabled;
}
//------------------------------------------------------------------------------
bool Dict::open(std::string filename)
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
    mContent.reset(new std::string(move(tmp)));
    mIs_open = true;
    mErrorState = false;


    return true;
}
//-----------------------------------------------------------------------------------
bool Dict::is_open()
{
    return mIs_open;
}
//-----------------------------------------------------------------------------------
std::shared_ptr<const std::string> Dict::getContens() const
{
    if (not mIs_open)
        throw std::domain_error{
            "Dictionary \"" + mFilename + "\" was not loaded."};
    return mContent;
}
//-----------------------------------------------------------------------------------
long long Dict::getContensSize() const
{
    return mContent->size();
}
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------

bool Dict::addWord(const std::string& word, const std::string& translation)
{
    if (not mIs_open)
        this->open(mFilename);
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
    mContent.reset(new string(move(tmp)));

    this->saveDictionary();

    return true;
}
//------------------------------------------------------------------------------
bool Dict::hasWord(const std::string& word)
{
    std::istringstream iss{*mContent};
    for (std::string line; std::getline(iss, line);)
        if (line == word)
            return true;
    return false;
}
//-----------------------------------------------------------------------------
void Dict::changeWord(
    const std::string& word, const std::string& newTranslation)
{
    // todo
    std::istringstream iss{*mContent};
    string output{""};
    string translation = newTranslation;
    std::replace(translation.begin(), translation.end(), '\n', ';');
    for (std::string line; std::getline(iss, line);)
    {
        if (line == word)
        {
            output += line + "\n";
            output += " " + translation + "\n";
            while (true)
            {

                if (std::getline(iss, line))
                    break;
                if (line[0] == ' ')
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
}
//-----------------------------------------------------------------------------
void Dict::saveDictionary()
{
    if (mFilename == "")
        return;
    std::ofstream outfile{mFilename};
    outfile << *mContent << endl;
}
//------------------------------------------------------------------------------


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
    REQUIRE(d.hasWord("ein") == true);
    REQUIRE(d.hasWord("zwei") == true);
    REQUIRE(d.hasWord("drei") == true);
    REQUIRE(d.hasWord("drei ") == false);
    REQUIRE(d.hasWord("one") == false);
    REQUIRE(d.hasWord("three") == false);
    REQUIRE(d.hasWord("one") == false);
    REQUIRE(d.hasWord("one") == false);
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
}
#endif
