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
#include "Search.h"

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

int main(int argc, char const* argv[])
{
    bool verbose {false};
    bool performanceTest {false};
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
        else if(tmp == "-p"){
            performanceTest = true;
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
    initTimer.end();


    SpeedTimer execTimer{true};
    workerResult results = _search(dicts, numthreads, words, verbose);
    if (performanceTest)
    {
        for (int i = 0; i < 10; ++i)
        {
            results = _search(dicts, numthreads, words, verbose);
            // if(results.empty())
            //     return 0;
        }
    }
    execTimer.end();



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

    completeTimer.end();
    if(verbose)
    {
        cout<<endl<<"Speed results:"<<endl;
        cout<<"  init = "<<initTimer.str()<<endl;
        cout<<"  exec = "<<execTimer.str()<<endl;
        cout<<"  Copmlete  = "<<completeTimer.str()<<endl;
    }
}