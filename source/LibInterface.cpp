#include "Dict.h"
#include "LibInterface.h"
#include "Search.h"
#include "Processer.h"
#include "SpeedTimer.h"


#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <vector>



std::vector<std::pair<int, Dict>> dicts {};

using namespace std;

int numthreads {0};

char *data{nullptr};


int clearDictionaries()
{
    int num = dicts.size();
    dicts.clear();
    return num;
}
//-----------------------------------------------------------------------------------
bool addDictionaryForce(const char *filename, bool priority)
{
    auto dict = std::find_if(dicts.begin(), dicts.end(), [&filename](auto &x){ return x.second.getFilename() == filename;});
    if(dict != dicts.end())
    {
        cout<<"xxxxxxxx = "<<dict->second.getFilename()<<endl;
        dict->first = priority;
        return dict->second.reload();
    }
    else
        addDictionary(filename, priority);
    return true;
}
//-----------------------------------------------------------------------------------
bool addDictionary(const char *filename, bool priority)
{
    if (any_of(dicts.begin(), dicts.end(), [&filename](auto& x) {
            return x.second.getFilename() == filename; }))
    {
        return true;
    }

    dicts.emplace_back();
    if (priority)
        dicts.back().first = -1;
    if(!dicts.back().second.open(filename))
    {
        dicts.pop_back();
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------------
void setNumberOfThreads(int x)
{
    if(x > 0)
    {
        numthreads = x;
    }
}
//-----------------------------------------------------------------------------------
char* search(const char* words)
{
    SpeedTimer timer{true};
    Processer proc{words};
    auto words2 = proc.getAllWordsSmall();

    if(numthreads == 0)
        numthreads = std::thread::hardware_concurrency();

    workerResult results = _search(dicts, numthreads, words2, false);

    string ret{"{\"results\":[ "};
    for(auto&& w : words2)
    {
        auto& rr = results[w];
        ret.append(string("{\"word\":\"")+w+"\",\"matches\":[ ");
        for(auto&& r : rr)
        {
            ret.append(string("\"")+r.words+"\",");
            // cout<<"  "<<r.score<<" -"<<r.words<<endl;
        }
        ret.pop_back();
        ret.append("]");
        if(rr.size() > 0)
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
    data = (char*)malloc(strlen(ret.c_str()) + 1);
    strcpy(data, ret.c_str());

    return data;
}
//-----------------------------------------------------------------------------------


