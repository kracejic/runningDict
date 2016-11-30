#include "Search.h"
#include <algorithm>
#include <numeric>

using namespace std;


workerResult _search(std::vector<Dict>& dicts, int numthreads,
    const std::vector<string>& words, bool verbose)
{
    if (verbose)
    {
        cout << "Threads:" << numthreads << endl;
        cout << "Dicts:" << dicts.size() << endl;

        cout << "Words to process:" << endl << "  ";
        for (auto&& i : words)
            cout << i << ", ";
        cout << endl << endl;
    }

    // Calculating size of all dicts
    long long sum = accumulate(
        dicts.begin(), dicts.end(), 0, [](long long sum, const auto& dict) {
            return (long long)(sum + dict.getContensSize());
        });
    // calculating threads needed for dicts
    vector<long long> threadsForDict;
    for (auto&& dict : dicts)
    {
        if (not dict.is_enabled())
        {
            threadsForDict.push_back(0); // push empty one ;-)
            continue;
        }
        threadsForDict.push_back(std::max(1ll,
            (((numthreads) * (long long)dict.getContens()->size()) / sum)));
        if (verbose)
            cout << "* Dict has x threads: " << threadsForDict.back() << endl;
    }


    // launch testing
    vector<future<std::map<std::string, std::vector<Result>>>> fut;
    for (int dictI = 0; dictI < (int)dicts.size(); ++dictI)
    {
        auto& dict = dicts[dictI];
        if (not dict.is_enabled())
            continue;
        long long size = dict.getContens()->size();
        for (int i = 0; i < threadsForDict[dictI]; ++i)
        {
            long long start = (i * size) / threadsForDict[dictI];
            long long end   = ((i + 1) * size) / threadsForDict[dictI];
            fut.push_back(
                std::async(std::launch::async, [&words, &dict, start, end]() {
                    Worker worker{dict};
                    return worker.search(words, start, end);
                }));
        }
    }


    // collect results
    workerResult results;
    for (auto&& worker : fut)
        for (auto&& wordRes : worker.get())
            results[wordRes.first].insert(results[wordRes.first].end(),
                wordRes.second.begin(), wordRes.second.end());


    // Process results
    for (auto&& rr : results)
    {
        // sort
        sort(rr.second.begin(), rr.second.end(), [](auto& x, auto& y) {
            if (x.score > y.score)
                return false;
            else if (x.score < y.score)
                return true;
            else
                return x.words < y.words;

        });
        // erase doubles
        rr.second.erase(unique(rr.second.begin(), rr.second.end(),
                            [](const auto& x, const auto& y) {
                                return x.words == y.words && x.score == y.score;
                            }),
            rr.second.end());
        // leave only best and best+1 matches for each word
        int best = rr.second.begin()->score;
        rr.second.erase(
            find_if(rr.second.begin(), rr.second.end(),
                [best](const auto& x) { return x.score > (best + 1); }),
            rr.second.end());
    }


    return results;
}
