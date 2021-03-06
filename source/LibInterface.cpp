#include "LibInterface.h"
#include "Dict.h"
#include "Processer.h"
#include "Search.h"
#include "SpeedTimer.h"


#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>


std::vector<Dict> dicts{};

using namespace std;

int numthreads{0};

char* data{nullptr};


int clearDictionaries()
{
    int num = dicts.size();
    dicts.clear();
    return num;
}
//-----------------------------------------------------------------------------------
bool addDictionaryForce(const char* filename, bool priority)
{
    auto dict = std::find_if(dicts.begin(), dicts.end(),
        [&filename](auto& x) { return x.getFilename() == filename; });
    if (dict != dicts.end())
    {
        dict->mBonus = int(priority);
        return dict->reload();
    }
    else
        addDictionary(filename, priority);
    return true;
}
//-----------------------------------------------------------------------------------
bool addDictionary(const char* filename, bool priority)
{
    if (any_of(dicts.begin(), dicts.end(),
            [&filename](auto& x) { return x.getFilename() == filename; }))
    {
        return true;
    }

    dicts.emplace_back();
    if (priority)
        dicts.back().mBonus = -1;
    if (!dicts.back().open(filename))
    {
        dicts.pop_back();
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------------
void setNumberOfThreads(int numThreads)
{
    if (numThreads > 0)
    {
        numthreads = numThreads;
    }
}
//-----------------------------------------------------------------------------------
char* search(const char* words)
{
    SpeedTimer timer{true};
    auto words2 = Processer::splitToWords(words);

    if (numthreads == 0)
        numthreads = std::thread::hardware_concurrency();
    if (numthreads == 0)
        numthreads = 1;

    workerResult results = _search(dicts, numthreads, words2, false);

    string ret{"{\"results\":[ "};
    for (auto&& w : words2)
    {
        auto& rr = results[w];
        ret.append(string("{\"word\":\"") + w + "\",\"matches\":[ ");
        for (Result& r : rr)
        {
            ret.append(string("[\"") + r.match + "\",\"" + r.words + "\",\"" +
                       to_string(r.score) + "\",\"" + r.dictFilename + "\"],");
            // cout<<"  "<<r.score<<" -"<<r.words<<endl;
        }
        ret.pop_back();
        ret.append("]");
        if (not rr.empty())
            ret.append(",\"score\":"s + to_string(rr[0].score));
        ret.append("},");
    }
    ret.pop_back();
    ret.append("], \"speed\":");
    timer.end();
    ret.append(to_string(timer.getSec()));
    ret.append("}");

    if (data != nullptr)
        free(data);
    data = static_cast<char*>(malloc(strlen(ret.c_str()) + 1));
    strncpy(data, ret.c_str(), strlen(ret.c_str()) + 1);

    return data;
}
//-----------------------------------------------------------------------------------
