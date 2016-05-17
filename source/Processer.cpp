#include "Processer.h"
#include <sstream>
#include <iostream>
#include <cctype>
#include <stdio.h>

// #include <clocale>

using namespace std;

using namespace Processer;

class Word
{
public:
    std::string text;
    Word(const std::string& txt);

    std::vector<std::string> words1;

    std::vector<std::string> getAllWordsBig();
    std::vector<std::string> getAllWordsSmall();
};


string getLowerCase(const string& txt)
{
    string res {txt};
    for(auto&& i : res)
    {
        if((unsigned char)i < 128 )
            i = tolower(i);
    }
    return res;
}

bool myIsPunct(char ch)
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
        case '~':
        case ' ':
        case '\t':
            return true;
        default:
            return false;
    }
}


bool myIsUpper(char ch)
{
    if((unsigned char)(ch) >= 128)
        return false;
    else
        return std::isupper(ch);
}


Word::Word(const std::string& txt) : text(txt)
{
    int it1 = 0, it2 = 1;
    bool wasUpperCase = false;
    //handle first character upper case
    if(myIsUpper(txt[1]))
        wasUpperCase = true;

    //fix CObjectThing to parse as ObjectThing
    if(txt[0] == 'C' && myIsUpper(txt[1])){
        it1 = 1;
        it2 = 2;
    }

    if(myIsPunct(txt[0]))
        ++it1;

    for ( ; it2 < (int)txt.size(); ++it2)
    {
        // printf(" + '%u' '%c' (%d, %d), upper=%d\n", (unsigned char)txt[it2],txt[it2], it1, it2, wasUpperCase);
        if(myIsUpper(txt[it2])){
            // printf("   isupper\n");
            if(!wasUpperCase) {
                words1.push_back(getLowerCase(txt.substr(it1, it2 - it1)));
                // printf("   catch  %d %d - %s\n", it1, it2, words1.back().c_str());
                it1 = it2;
                wasUpperCase = true;
            }
        }
        else
        {
            // printf("   not isupper\n");
            if(myIsPunct(txt[it2])){
                if(it2 > 1)
                {
                    words1.push_back(getLowerCase(txt.substr(it1, it2 - it1)));
                    // printf("   catch3 %d %d - %s\n", it1, it2, words1.back().c_str());
                }
                do{
                    ++it2;
                } while (myIsPunct(txt[it2]) or it2 == (int)txt.size());
                --it2;
                it1 = it2+1;
                // printf("   catch3end %d %d / %d\n", it1, it2,(int)txt.size());
                wasUpperCase = true;
            }
            else{
                // printf("     not\n");
                if(wasUpperCase && ((it2 - it1) > 1))
                {
                    words1.push_back(getLowerCase(txt.substr(it1, it2 - it1 - 1)));
                    // printf("   catch2 %d %d - %s\n", it1, it2, words1.back().c_str());
                    --it2;
                    it1 = it2;
                }
                wasUpperCase = false;
            }
        }
        // printf(" - '%u' (%d, %d), upper=%d\n", (unsigned char)txt[it2], it1, it2, wasUpperCase);
    }
    if(it2 != (it1))
    {
        if(not myIsPunct(txt[it1]))
        {
            words1.emplace_back(getLowerCase(txt.substr(it1, it2)));
            // printf("   last   %d %d - %s\n\n", it1, it2, words1.back().c_str());
        }
    }

    // cout<<"@words = ";
    // for (auto& word : words1)
    //     cout<<"'"<<word<<"', ";
    // cout<<endl<<endl;

}
//-----------------------------------------------------------------------------------
/**
 * Returns list of small splitted up words
 */
std::vector<std::string> Word::getAllWordsSmall()
{
    std::vector<std::string> res;
    res.reserve(words1.size());
    res.insert(res.end(), words1.begin(), words1.end());
    return res;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------


std::vector<std::string> Processer::splitToWords(char const *text) {
    std::vector<Word> words;

    //process words
    [&words, text]()
    {
        istringstream txt{text};
        while(txt)
        {
            string word;
            txt >> word;
            if(word.size() > 0)
                words.emplace_back(word);
        }
    }();


    //construct result
    std::vector<std::string> res;
    for(auto&& w : words)
    {
        std::vector<std::string> add = w.getAllWordsSmall();
        res.insert(res.end(), add.begin(), add.end());
    }
    return res;}
//------------------------------------------------------------------------------
std::vector<std::string> Processer::splitToWords(int startIndex, int endIndex,
                                      char const *array[])
{
    std::vector<Word> words;

    //process words
    for (int i = startIndex; i < endIndex; ++i)
    {
        istringstream txt(array[i]);
        while(txt)
        {
            string word;
            txt >> word;
            if(word.size() > 0)
                words.emplace_back(word);
        }
    }
    // cout<<"start: "<<startIndex<<", endIndex: "<<endIndex<<endl;
    // cout<<"words size: "<<words.size()<<endl;

    //construct result
    std::vector<std::string> res;
    for(auto&& w : words)
    {
        std::vector<std::string> add = w.getAllWordsSmall();
        res.insert(res.end(), add.begin(), add.end());
    }
    return res;
}
//------------------------------------------------------------------------------


#ifdef UNIT_TESTS
#include "test/catch.hpp"
TEST_CASE("Check splitting of strings 1 - dots"){
    REQUIRE(Processer::splitToWords(".").size() == 0);
    REQUIRE(Processer::splitToWords("..").size() == 0);
    REQUIRE(Processer::splitToWords("...").size() == 0);
    REQUIRE(Processer::splitToWords("....").size() == 0);
    REQUIRE(Processer::splitToWords(".....").size() == 0);
    REQUIRE(Processer::splitToWords("... ..").size() == 0);
    REQUIRE(Processer::splitToWords(".. . ..").size() == 0);
    REQUIRE(Processer::splitToWords(".. .. ..").size() == 0);
    REQUIRE(Processer::splitToWords(".. ... .").size() == 0);
    REQUIRE(Processer::splitToWords(".. .. ..").size() == 0);
    REQUIRE(Processer::splitToWords(".. .. ...").size() == 0);
    REQUIRE(Processer::splitToWords(".. .. ....").size() == 0);
    REQUIRE(Processer::splitToWords(" .. .. .... ").size() == 0);
    REQUIRE(Processer::splitToWords(" .. .. ....   ").size() == 0);
}


TEST_CASE("Check splitting of strings 2 - Katze+dots"){
    auto res = Processer::splitToWords("Katze.");
    REQUIRE(res.size() == 1);
    REQUIRE(res[0] == "katze");

    auto res2 = Processer::splitToWords("Katze..");
    REQUIRE(res2.size() == 1);
    REQUIRE(res2[0] == "katze");

    auto res3 = Processer::splitToWords("Katze...");
    REQUIRE(res3.size() == 1);
    REQUIRE(res3[0] == "katze");
}

TEST_CASE("Check splitting of strings 3 complex"){
    auto res = Processer::splitToWords("CKleineKatze. _LUMPYSofar....kEin");
    REQUIRE(res.size() == 6);
    REQUIRE(res[0] == "kleine");
    REQUIRE(res[1] == "katze");
    REQUIRE(res[2] == "lumpy");
    REQUIRE(res[3] == "sofar");
    REQUIRE(res[4] == "k");
    REQUIRE(res[5] == "ein");
}


TEST_CASE("Check splitting of strings 4 complex"){
    auto res = Processer::splitToWords("......CTestIst_eine_wichtige_ja.....");
    REQUIRE(res.size() == 6);
    REQUIRE(res[0] == "c");
    REQUIRE(res[1] == "test");
    REQUIRE(res[2] == "ist");
    REQUIRE(res[3] == "eine");
    REQUIRE(res[4] == "wichtige");
    REQUIRE(res[5] == "ja");
}

TEST_CASE("Check splitting of strings 5 complex"){
    auto res = Processer::splitToWords("CKeine_katze_TCP_Tag_CPUUnit_KatzeUnd ihre");
    REQUIRE(res.size() == 9);
    REQUIRE(res[0] == "keine");
    REQUIRE(res[1] == "katze");
    REQUIRE(res[2] == "tcp");
    REQUIRE(res[3] == "tag");
    REQUIRE(res[4] == "cpu");
    REQUIRE(res[5] == "unit");
    REQUIRE(res[6] == "katze");
    REQUIRE(res[7] == "und");
    REQUIRE(res[8] == "ihre");
}

#endif