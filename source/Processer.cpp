#include "Processer.h"
#include <sstream>
#include <iostream>
#include <cctype>
// #include <clocale>

using namespace std;


string getLowerCase(const string& txt)
{
    string res {txt};
    for(auto&& i : res)
    {
        i = tolower(i);
    }
    return res;
}


Word::Word(const std::string& txt) : text(txt)
{
    int it1 = 0, it2 = 1;
    bool wasUpperCase = false;
    //handle first character upper case
    if(std::isupper(txt[1]))
        wasUpperCase = true;

    //fix CObjectThing to parse as ObjectThing
    if(txt[0] == 'C' && std::isupper(txt[1])){
        it1 = 1;
        it2 = 2;
    }

    for ( ; it2 < (int)txt.size(); ++it2)
    {
        if(std::isupper(txt[it2])){
            if(!wasUpperCase) {
                words1.push_back(getLowerCase(txt.substr(it1, it2 - it1)));
                // printf("   catch  %d %d - %s\n", it1, it2, words1.back().c_str());
                it1 = it2;
                wasUpperCase = true;
            }
        }
        else
        {
            if(std::ispunct(txt[it2])){
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
Processer::Processer(char const* text)
{
    istringstream txt(text);
    while(txt)
    {
        string word;
        txt >> word;
        if(word.size() > 0) {
            words.emplace_back(word);
        }
    }
}
Processer::Processer(int argstart, int argc, char const* argv[])
{
    for (int i = argstart; i < argc; ++i)
    {
        // cout<<"working: "<<argv[i]<<endl;
        istringstream txt(argv[i]);
        while(txt)
        {
            string word;
            txt >> word;
            if(word.size() > 0) {
                words.emplace_back(word);
            }
        }

    }
    // cout<<"words size: "<<words.size()<<endl;

}
//-----------------------------------------------------------------------------------
std::vector<std::string> Processer::getAllWordsBig()
{
    std::vector<std::string> res;
    for(auto&& w : words)
    {
        std::vector<std::string> add = w.getAllWordsBig();
        res.insert(res.end(), add.begin(), add.end());
    }
    return res;
}
//-----------------------------------------------------------------------------------

/**
 * Returns list of small splitted up words
 */
std::vector<std::string> Processer::getAllWordsSmall()
{
    std::vector<std::string> res;
    for(auto&& w : words)
    {
        std::vector<std::string> add = w.getAllWordsSmall();
        res.insert(res.end(), add.begin(), add.end());
    }
    return res;
}
//-----------------------------------------------------------------------------------

