#include "Search.h"
#include <algorithm>
#include <numeric>

using namespace std;


workerResult _search(std::vector<std::pair<int, Dict>>& dicts,
                    int numthreads, const std::vector<string>& words, bool verbose)
{
    if(verbose)
    {
        cout<<"Threads:"<<numthreads<<endl  ;
        cout<<"Dicts:"<<dicts.size()<<endl  ;

        cout<<"Words to process:"<<endl<<"  ";
        for(auto&& i : words)
            cout<<i<<", ";
        cout<<endl<<endl;
    }


    long long sum = accumulate(dicts.begin(), dicts.end(), 0,
        [](long long sum, const auto& x)
        {
            return (long long)(sum + x.second.getContens().size());
        });
    vector<long long> threadsForDict;
    for(auto&& dict : dicts)
    {
        threadsForDict.push_back(
            1 + (((numthreads - 1) * (long long)dict.second.getContens().size()) / sum));
        if(verbose)
            cout<<"* Dict has x threads: "<<threadsForDict.back()<<endl;
    }


    // launch testing
    vector<future<std::map<std::string, std::vector<Result>>>> fut;
    for(int dictI = 0; dictI < (int)dicts.size(); ++dictI)
    {
        auto& dict = dicts[dictI];
        long long size = dict.second.getContens().size();
        for(int i = 0; i < threadsForDict[dictI]; ++i)
        {
            long long start = (i * size) / threadsForDict[dictI];
            long long end = ((i + 1) * size) / threadsForDict[dictI];
            fut.push_back(
                std::async(std::launch::async, [&words, &dict, start, end]()
                    {
                        Worker worker{dict.second, dict.first};
                        return worker.search(words, start, end);
                    }));
        }
    }


    // collect results
    workerResult results;
    for(auto&& worker : fut)
        for(auto&& wordRes : worker.get())
            results[wordRes.first].insert(results[wordRes.first].end(),
                wordRes.second.begin(), wordRes.second.end());



    // Process results
    for(auto&& rr : results)
    {
        //sort
        sort(rr.second.begin(), rr.second.end(), [](auto& x, auto& y)
             {
                if(x.score > y.score)
                    return false;
                else if(x.score < y.score)
                    return true;
                else
                    return x.words < y.words;

             });
        //erase doubles
        rr.second.erase(unique(rr.second.begin(), rr.second.end(),
                               [](const auto& x, const auto& y)
                               {
                                   return x.words == y.words && x.score == y.score;
                               }),
                        rr.second.end());
        //leave only best matches for each word
        int max = 5;
        rr.second.erase(find_if(rr.second.begin(), rr.second.end(),
                                [max](const auto& x) mutable
                                {
                                    bool ret = max < x.score;
                                    max = x.score;
                                    return ret;
                                }),
                        rr.second.end());
    }




    return results;
}
