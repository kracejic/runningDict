#include "Worker.h"
#include <algorithm>
#include <algorithm>
#include <codecvt>
#include <iostream>
#include <locale>
#include <numeric>
#include <sstream>


using namespace std;

int levenshtein_distance(const std::u16string& s1, const std::u16string& s2)
{
    // To change the type this function manipulates and returns, change
    // the return type and the types of the two variables below.
    int s1len = s1.size();
    s1len = min(512, s1len);
    int s2len = s2.size();

    int column_start = (decltype(s1len))1;

    // auto column = new decltype(s1len)[s1len + 1];
    int column[512];
    std::iota(column + column_start, column + s1len + 1, column_start);

    for (int x = column_start; x <= s2len; x++)
    {
        column[0] = x;
        int last_diagonal = x - column_start;
        int y;
        // cout<<"num"<<x<<" = "<<endl<<"  ";
        for (y = column_start; y <= s1len; y++)
        {
            int old_diagonal = column[y];
            auto possibilities = {column[y] + 1, column[y - 1] + 1,
                last_diagonal + (s1[y - 1] == s2[x - 1] ? 0 : 1)};
            column[y] = std::min(possibilities);
            last_diagonal = old_diagonal;
            // cout<<"("<<y<<"="<<column[y]<<" ) ";
        }
        // cout<<endl<<endl;

        if (x == 4)
            if (column[4] >= 4)
            {
                column[s1len] = 100;
                break;
            }
    }
    int result = column[s1len];
    // delete[] column;
    return result;
}

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template <class Facet>
struct deletable_facet : Facet
{
    template <class... Args>
    deletable_facet(Args&&... args)
        : Facet(std::forward<Args>(args)...)
    {
    }
    ~deletable_facet()
    {
    }
};

//-----------------------------------------------------------------------------
workerResult Worker::search(const Dict& mDict,
    const std::vector<std::string>& wordsIn, long long start, long long end)
{
    workerResult result;
    std::shared_ptr<const string> dictHolder = mDict.Dict::getContens();
    int mBonus = mDict.mBonus;
    std::istringstream text(*dictHolder);

    // convert words to char16
    wstring_convert<
        deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t>
        utfConvertor;
    std::vector<pair<u16string, int>> words;
    for (auto&& w : wordsIn)
    {
        // cout<<"  converting: \""<<w<<"\" size="<<w.size()<<endl;
        words.emplace_back(utfConvertor.from_bytes(w), (2 * w.size()) / 3 + 1);
    }

    // some overlap is neccessary for start if not starting from the beginning
    if (start > 0)
    {
        start = (start > 256) ? (start - 256) : 0;
        text.seekg(start);
        string tmp;
        do
        { // reset line position
            getline(text, tmp);
        } while (tmp[0] == ' ');
    }
    if (end > (int)dictHolder->size())
        end = dictHolder->size();


    string german, firstLine, english, newLine;
    u16string german2;
    if (!getline(text, firstLine))
        return {};
    bool cont = true;
    while (cont)
    {
        if (!getline(text, english))
            break;
        if (getline(text, newLine))
        {
            while (newLine[0] == ' ')
            {
                english.append(newLine);
                if (!getline(text, newLine))
                {
                    cont = false;
                    break;
                }
            }
        }
        else
            cont = false;
        german = firstLine.substr(0, firstLine.find('/'));
        if (german[german.size() - 1] == ' ')
            german.resize(german.size() - 1);

        // cout << "  testing:" << german << endl;
        for (auto&& w : words)
        {
            german2 = utfConvertor.from_bytes(german);
            int dist = 2 * levenshtein_distance(w.first, german2);
            if (dist < w.second)
            {
                // cout << "     *- (" << dist << ")" <<
                // utfConvertor.to_bytes(w.first)
                //      << " = " << german << " = " << english << endl;
                result[utfConvertor.to_bytes(w.first)].emplace_back(
                    mBonus + dist,
                    english.c_str() + 1, // first character is space
                    german, mDict.getFilename());
                // result[utfConvertor.to_bytes(w.first)].emplace_back(english);
            }
        }


        firstLine = newLine;
        if (text.tellg() > end)
            break;
    }

    for (auto&& w : result)
        sort(w.second.begin(), w.second.end(),
            [](auto& x, auto& y) { return x.score < y.score; });


    return result;
}
//-----------------------------------------------------------------------------------
