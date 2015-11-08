#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <future>

#include "version.h"
#include "Dict.h"
#include "Worker.h"

using namespace std;

void printHelp()
{
    cout << "Help for cleanCppApplication " << endl;
    cout << "Version: " << Version::getVersionLong() << endl;
    cout << "Author:  " << endl;
    cout << "URL:     " << endl;
}

int main(int argc, char const* argv[])
{
    std::vector<std::pair<int, Dict>> dicts;
    std::vector<string> words;


    // Print help if no arguments are given
    if(argc == 1)
        printHelp();

    // process parameters
    int argIt;
    for(argIt = 1; argIt < argc; ++argIt)
    {
        string tmp = argv[argIt];

        if(tmp == "--help" || tmp == "-h")
            printHelp();
        else if(tmp == "--version")
            cout << "v" << Version::getVersionShort() << endl;
        else if(tmp == "-d")
        {
            if(argIt+1 < argc)
            {
                argIt++;
                dicts.emplace_back();
                if(!dicts.back().second.open(argv[argIt]))
                {
                    cout<<"Error opening "<<argv[argIt]<<" file."<<endl;
                    return 1;
                }
            }
        }
        else if(tmp == "-D")
        {
            if(argIt+1 < argc)
            {
                argIt++;
                dicts.emplace_back();
                dicts.back().first = -1;
                if(!dicts.back().second.open(argv[argIt]))
                {
                    cout<<"Error opening "<<argv[argIt]<<" file."<<endl;
                    return 1;
                }
            }
        }
        else
            break;

    }
    // process rest of the free arguments. EG. file list, word list
    for(; argIt < argc; ++argIt)
        words.emplace_back(argv[argIt]);


    // launch testing
    vector<future<std::map<std::string, std::vector<Result>>>> fut;
    for(auto&& dict : dicts)
    {
        fut.push_back(
            std::async(std::launch::async, [&words, &dict]()
                       {
                           Worker worker{dict.second, dict.first};
                           return worker.search(words);
                       }));
    }


    // collect results
    workerResult results;
    for(auto&& worker : fut)
        for(auto&& wordRes : worker.get())
            results[wordRes.first].insert(results[wordRes.first].end(),
                                          wordRes.second.begin(),
                                          wordRes.second.end());


    // Process results
    for(auto&& rr : results)
    {
        //sort
        sort(rr.second.begin(), rr.second.end(), [](auto& x, auto& y)
             {
                 return x.score < y.score;
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

    cout<<"-----RESULTS-----"<<endl;
    //print results
    for(auto&& w : words)
    {
        auto& rr = results[w];
        cout<<w<<endl;
        for(auto&& r : rr)
        {
            cout<<"  "<<r.score<<" -"<<r.words<<endl;
        }
    }


    return 0;
}
