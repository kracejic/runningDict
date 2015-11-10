#include "Worker.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <locale>
#include <codecvt>
#include <algorithm>


using namespace std;

int levenshtein_distance(const std::u16string& s1, const std::u16string& s2)
{
    // To change the type this function manipulates and returns, change
    // the return type and the types of the two variables below.
    int s1len = s1.size();
    s1len = min(512, s1len);
    int s2len = s2.size();

    auto column_start = (decltype(s1len))1;

    // auto column = new decltype(s1len)[s1len + 1];
    int column[512];
    std::iota(column + column_start, column + s1len + 1, column_start);

    for(auto x = column_start; x <= s2len; x++)
    {
        column[0] = x;
        auto last_diagonal = x - column_start;
        int y;
        // cout<<"num"<<x<<" = "<<endl<<"  ";
        for(y = column_start; y <= s1len; y++)
        {
            auto old_diagonal = column[y];
            auto possibilities
                = {column[y] + 1, column[y - 1] + 1,
                   last_diagonal + (s1[y - 1] == s2[x - 1] ? 0 : 1)};
            column[y] = std::min(possibilities);
            last_diagonal = old_diagonal;
            // cout<<"("<<y<<"="<<column[y]<<" ) ";
        }
        // cout<<endl<<endl;

        if(x == 4)
            if(column[4] >= 4)
            {
                column[s1len] = 100;
                break;
            }

    }
    auto result = column[s1len];
    // delete[] column;
    return result;
}

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template <class Facet> struct deletable_facet : Facet
{
    template <class... Args>
    deletable_facet(Args&&... args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};

workerResult Worker::search(const std::vector<std::string>& wordsIn)
{
    workerResult result;
    long long position{mStart};
    std::istringstream text(mDict.Dict::getContens());

    // convert words to char16
    wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>,
                    char16_t> utfConvertor;
    std::vector<u16string> words;
    for(auto&& w : wordsIn)
    {
        // cout<<"  converting: \""<<w<<"\" size="<<w.size()<<endl;
        words.emplace_back(utfConvertor.from_bytes(w));
    }

    //some overlap is neccessary for start if not starting from the beginning
    if(mStart > 0)
    {
        mStart = (mStart > 256) ? (mStart - 256) : 0;
        text.seekg(mStart);
        string tmp;
        do { // reset line position
            getline(text, tmp);
        } while(tmp[0] == ' ');
    }
    if(mEnd > mDict.getContens().size())
        mEnd = mDict.getContens().size();


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
            if(dist < (w.size()/3+2))
            {
                // cout << "     *- (" << dist << ")" << utfConvertor.to_bytes(w)
                //      << " = " << german << " = " << english << endl;
                result[utfConvertor.to_bytes(w)].emplace_back(mBonus+dist, english);
                // result[utfConvertor.to_bytes(w)].emplace_back(english);
            }
        }



        firstLine = newLine;
        if(text.tellg() > mEnd)
            break;
    }

    for(auto&& w : result)
        sort(w.second.begin(), w.second.end(),
                [](auto& x, auto& y) {return x.score < y.score; }
                );


    return result;
}
//-----------------------------------------------------------------------------------
workerResult Worker::search(const std::vector<std::string>& words,
                            long long start, long long end)
{
    mStart = start;
    mEnd = end;
    return search(words);
}
//-----------------------------------------------------------------------------------
