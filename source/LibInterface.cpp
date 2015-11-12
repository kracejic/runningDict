#include "Dict.h"
#include "LibInterface.h"
#include "Search.h"
#include "Processer.h"


#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <algorithm>


std::vector<std::pair<int, Dict>> dicts {};

using namespace std;

char *data{nullptr};

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
char* search(const char* words)
{
    Processer proc{words};
    auto words2 = proc.getAllWordsSmall();
    workerResult results = _search(dicts, 8, words2, false);
        // std::vector<std::pair<int, Dict>>& dicts,
    //                  int numthreads, const std::vector<std::string>& words,
    //                  bool verbose);

    string ret{"[ "};
    for(auto&& w : words2)
    {
        auto& rr = results[w];
        ret.append(string("{\"")+w+"\":[ ");
        for(auto&& r : rr)
        {
            ret.append(string("\"")+r.words+"\",");
            // cout<<"  "<<r.score<<" -"<<r.words<<endl;
        }
        ret.pop_back();
        ret.append("]},");
    }
    ret.pop_back();
    ret.append("]");

    if (data != nullptr)
        free(data);
    data = (char*)malloc(strlen(ret.c_str()) + 1);
    strcpy(data, ret.c_str());

    return data;
}
//-----------------------------------------------------------------------------------

int test()
{
    return 2;
}
//-----------------------------------------------------------------------------------

