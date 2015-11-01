#include "Worker.h"
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace std;



// len_s and len_t are the number of characters in string s and t respectively
int LevenshteinDistance(const string& s, int len_s, const string& t, int len_t)
{
    int cost;
    static long num = 0;
    num++;
    cout<<num<<endl;

    /* base case: empty strings */
    if(len_s == 0)
        return len_t;
    if(len_t == 0)
        return len_s;

    /* test if last characters of the strings match */
    if(s[len_s - 1] == t[len_t - 1])
        cost = 0;
    else
        cost = 1;

    /* return minimum of delete char from s, delete char from t, and delete char
     * from both */
    return std::min({LevenshteinDistance(s, len_s - 1, t, len_t) + 1,
                   LevenshteinDistance(s, len_s, t, len_t - 1) + 1,
                   LevenshteinDistance(s, len_s - 1, t, len_t - 1) + cost});
}

int LevenshteinDistance(const string& s, const string& t){
    return LevenshteinDistance(s, (int)s.size(), t, (int)t.size());
}





// TWO
int levenshtein_distance(const std::string &s1, const std::string &s2)
{
    // To change the type this function manipulates and returns, change
    // the return type and the types of the two variables below.
    int s1len = s1.size();
    int s2len = s2.size();

    auto column_start = (decltype(s1len))1;

    auto column = new decltype(s1len)[s1len + 1];
    std::iota(column + column_start, column + s1len + 1, column_start);

    for (auto x = column_start; x <= s2len; x++) {
        column[0] = x;
        auto last_diagonal = x - column_start;
        for (auto y = column_start; y <= s1len; y++) {
            auto old_diagonal = column[y];
            auto possibilities = {
                column[y] + 1,
                column[y - 1] + 1,
                last_diagonal + (s1[y - 1] == s2[x - 1]? 0 : 1)
            };
            column[y] = std::min(possibilities);
            last_diagonal = old_diagonal;
        }
    }
    auto result = column[s1len];
    delete[] column;
    return result;
}



std::map<std::string,std::vector<Result>> Worker::search(const std::vector<std::string>& words)
{
    cout<<"xx"<<endl;
    long long position {mStart};
    std::istringstream text(mDict.Dict::getContens());
    cout<<"xx"<<endl;

    // cout<<levenshtein_distance("testd", "test")<<endl;
    // return {};
    cout<<"words"<<endl;
    for(auto&& w : words)
        cout<<"  "<<w<<endl;

    string one, oneC, two;
    while (true)
    {
        text>>one;
        if(!getline(text, oneC))
            break;
        if(!getline(text, two))
            break;
        // cout<<one<<endl;

        for(auto&& w : words)
        {
            if(levenshtein_distance(w, one) < 2)
            {
                cout<<w<<" = "<<one<<endl;
            }
        }



    }
    cout<<"xx"<<endl;
    return {};
}
//-----------------------------------------------------------------------------------
std::map<std::string,std::vector<Result>> Worker::search(const std::vector<std::string>& words,
                                   long long start, long long end)
{
    mStart = start;
    mEnd = end;
    return search(words);
}
//-----------------------------------------------------------------------------------

