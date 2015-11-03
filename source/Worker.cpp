#include "Worker.h"
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace std;


int levenshtein_distance(const std::string& s1, const std::string& s2)
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
                   last_diagonal + (s1[y - 1] == s2[x - 1] ? 0 : 1)};
            column[y] = std::min(possibilities);
            last_diagonal = old_diagonal;
        }
    }
    auto result = column[s1len];
    delete[] column;
    return result;
}



std::map<std::string, std::vector<Result>>
    Worker::search(const std::vector<std::string>& words)
{
    cout << "xx" << endl;
    long long position{mStart};
    std::istringstream text(mDict.Dict::getContens());

    // cout<<levenshtein_distance("testd", "test")<<endl;
    // return {};
    cout << "Words" << endl;
    for(auto&& w : words)
        cout << "  " << w << endl;

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
        german = firstLine.substr(0, firstLine.find(' '));

        // cout << "  testing:" << german << endl;
        for(auto&& w : words)
        {
            if(levenshtein_distance(w, german) < 2)
            {
                cout << "     *-" << w << " = " << german << " = " << english
                     << endl;
            }
        }


        firstLine = newLine;
    }
    cout << "xx" << endl;
    return {};
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
