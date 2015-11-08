#include "Worker.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <locale>
#include <codecvt>


using namespace std;

template<typename T>
bool checkChars(T* c1, T* c2)
{
    return (*c1 == *c2 ? 0 : 1);
}

int levenshtein_distance(const std::u16string& s1, const std::u16string& s2)
{
    // To change the type this function manipulates and returns, change
    // the return type and the types of the two variables below.
    int s1len = s1.size();
    int s2len = s2.size();

    auto column_start = (decltype(s1len))1;

    auto column = new decltype(s1len)[s1len + 1];
    std::iota(column + column_start, column + s1len + 1, column_start);

    for(auto x = column_start; x <= s2len; x++)
    {
        column[0] = x;
        auto last_diagonal = x - column_start;
        for(auto y = column_start; y <= s1len; y++)
        {
            auto old_diagonal = column[y];
            auto possibilities
                = {column[y] + 1, column[y - 1] + 1,
                   last_diagonal + checkChars(&(s1[y - 1]), &(s2[x - 1]))};
            // auto possibilities
            //     = {column[y] + 1, column[y - 1] + 1,
            //        last_diagonal + (s1[y - 1] == s2[x - 1] ? 0 : 1)};
            column[y] = std::min(possibilities);
            last_diagonal = old_diagonal;
        }
    }
    auto result = column[s1len];
    delete[] column;
    return result;
}



std::map<std::string, std::vector<Result>>
    Worker::search(const std::vector<std::string>& wordsIn)
{
    std::map<std::string, std::vector<Result>> result;
    cout << "xx" << endl;
    long long position{mStart};
    std::istringstream text(mDict.Dict::getContens());



    // cout<<levenshtein_distance("testd", "test")<<endl;
    // return {};
    cout << "Words" << endl;

    wstring_convert<codecvt_utf8<char16_t>, char16_t> utfConvertor;
    std::vector<u16string> words;
    for(auto&& w : wordsIn)
    {
        cout << "  " << w << endl;
        words.emplace_back(utfConvertor.from_bytes(w));
    }



    cout << endl << "----Results----" << endl;
    string german, firstLine, english, newLine;
    if(!getline(text, firstLine))
        return {};
    bool cont = true;
    while(cont)
    {
        if(!getline(text, english))
            break;
        if(getline(text, newLine))
        {
            while(newLine[0] == ' ')
            {
                english.append(newLine);
                if(!getline(text, newLine))
                {
                    cont = false;
                    break;
                }
            }
        }
        else
            cont = false;
        german = firstLine.substr(0, firstLine.find('/'));
        if(german[german.size()-1] == ' ')
            german.resize(german.size()-1);

        // cout << "  testing:" << german << endl;
        u16string german2;
        for(auto&& w : words)
        {
            german2 = utfConvertor.from_bytes(german);
            int dist = levenshtein_distance(w, german2);
            if(dist < 2)
            {
                // cout << "     *- (" << dist << ")" << utfConvertor.to_bytes(w)
                //      << " = " << german << " = " << english << endl;
                result[utfConvertor.to_bytes(w)].emplace_back(dist, english);
                // result[utfConvertor.to_bytes(w)].emplace_back(english);
            }
        }


        firstLine = newLine;
    }
    cout << "xx" << endl;
    return result;
}
//-----------------------------------------------------------------------------------
std::map<std::string, std::vector<Result>>
    Worker::search(const std::vector<std::string>& words, long long start,
                   long long end)
{
    mStart = start;
    mEnd = end;
    return search(words);
}
//-----------------------------------------------------------------------------------
