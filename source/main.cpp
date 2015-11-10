#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <cstdio>
#include <future>

#include "version.h"
#include "Dict.h"
#include "Worker.h"
#include "Processer.h"
#include "SpeedTimer.h"

using namespace std;

void printHelp()
{
    cout << "Help for runningDict " << endl;
    cout << "Version: " << Version::getVersionLong() << endl << endl;

    string help = R"delimiterFoo(
    -d [file]     - dictionary file
    -D [file]     - dictionary with higher priority, gets also partial words
    -j[num]       - number of threads (default = 3)
    -h --help     - prints help
    --in          - after this command all others are handled like input arguments

Usage exaples:

    ./runMe -j8 -d freedict-deu-eng.dict -D test.dict Ihre deutsch ist CStrukturKeine

Parsing CamelCase:

    CCamelCaseTMUKing will be parsed to camel, case, tmu, king

    )delimiterFoo";

    cout<<help<<endl;
}

int finder(int argc, char const* argv[]);


int main(int argc, char const* argv[])
{
    return finder(argc, argv);
}

int finder(int argc, char const* argv[])
{
    bool verbose {false};
    SpeedTimer completeTimer{true};
    SpeedTimer initTimer{true};
    std::vector<std::pair<int, Dict>> dicts;

    // Print help if no arguments are given
    if(argc == 1)
        printHelp();

    // process parameters
    int argIt;
    int numthreads = std::thread::hardware_concurrency();
    numthreads = (numthreads > 1) ? numthreads : 1;
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
        else if(tmp[0] == '-' && tmp[1] == 'j')
        {
            sscanf(tmp.c_str(), "-j%d", &numthreads);
            numthreads = (numthreads > 0) ? numthreads : 1;
            // cout<<"THREADS: "<<numthreads<<endl;
        }
        else if(tmp == "-v"){
            verbose = true;
        }
        else if(tmp == "--in"){
            ++argIt;
            break;
        }
        else
            break;

    }
    // process rest of the free arguments. EG. file list, word list
    // for(; argIt < argc; ++argIt)
    //     words.emplace_back(argv[argIt]);
    Processer processer{argIt, argc, argv};
    std::vector<string> words = processer.getAllWordsSmall();



    if(verbose)
    {
        cout<<"Words to process:"<<endl<<"  ";
        for(auto&& i : words) 
            cout<<i<<", ";
        cout<<endl<<endl;
    }
    


    long long sum = accumulate(dicts.begin(), dicts.end(), 0,
                               [](long long sum, const auto& x) {
                                   return sum + x.second.getContens().size();
                               });
    vector<long long> threadsForDict;
    for(auto&& dict : dicts)
    {
        threadsForDict.push_back(
            1 + (((numthreads - 1) * (long long)dict.second.getContens().size())
                 / sum));
        // cout<<"  threads: "<<threadsForDict.back()<<endl;
    }

    initTimer.end();
    SpeedTimer execTimer{true};

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
            fut.push_back(std::async(std::launch::async,
                                     [&words, &dict, start, end]()
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
                                          wordRes.second.begin(),
                                          wordRes.second.end());


    execTimer.end();
    SpeedTimer finTimer{true};

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

    
    if(verbose)
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

    finTimer.end();
    completeTimer.end();
    if(verbose)
    {
        cout<<endl<<"Speed results:"<<endl;
        cout<<"  init = "<<initTimer.str()<<endl;
        cout<<"  exec = "<<execTimer.str()<<endl;
        cout<<"  fin  = "<<finTimer.str()<<endl<<endl;
        cout<<"  Copmlete  = "<<completeTimer.str()<<endl;
    }


    return 0;
}
