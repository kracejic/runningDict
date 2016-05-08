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
    std::vector<std::string> words2;
    std::vector<std::string> words3;

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

    for ( ; it2 < (int)txt.size(); ++it2)
    {
        // printf(" + '%u' (%d, %d), upper=%d\n", (unsigned char)txt[it2], it1, it2, wasUpperCase);
        if(myIsUpper(txt[it2])){
            if(!wasUpperCase) {
                words1.push_back(getLowerCase(txt.substr(it1, it2 - it1)));
                // printf("   catch  %d %d - %s\n", it1, it2, words1.back().c_str());
                it1 = it2;
                wasUpperCase = true;
            }
        }
        else
        {
            if(myIsPunct(txt[it2])){
                words1.push_back(getLowerCase(txt.substr(it1, it2 - it1)));
                // printf("   catch3 %d %d - %s\n", it1, it2, words1.back().c_str());
                ++it2;
                it1 = it2;
                wasUpperCase = true;
            }
            else{
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
        // printf(" - '%u' (%d, %d), upper=%d\n",
            // (unsigned char)txt[it2], it1, it2, wasUpperCase);
    }
    words1.emplace_back(getLowerCase(txt.substr(it1, it2)));
    // printf("   last   %d %d - %s\n\n", it1, it2, words1.back().c_str());

    for (int i = 0; i < ((int)words1.size())-1; i+=2)
        words2.emplace_back(words1[i]+words1[i+1]);
    for (int i = 1; i < ((int)words1.size())-1; i+=2)
        words2.emplace_back(words1[i]+words1[i+1]);
}
//-----------------------------------------------------------------------------------
std::vector<std::string> Word::getAllWordsBig()
{
    std::vector<std::string> res;

    res.reserve(1 + words1.size() + words2.size() + words3.size());
    res.push_back(this->text);
    res.insert(res.end(), words1.begin(), words1.end());
    res.insert(res.end(), words2.begin(), words2.end());
    res.insert(res.end(), words3.begin(), words3.end());
    return res;
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
