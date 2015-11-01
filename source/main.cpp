#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdio.h>

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
    Dict dict;
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
                if(!dict.open(argv[argIt]))
                    cout<<"Error opening "<<argv[argIt]<<" file."<<endl;
            }
        }
        else
            break;

    }
    // process rest of the free arguments. EG. file list, word list
    for(; argIt < argc; ++argIt)
    {
        cout << argv[argIt] << endl;
        words.emplace_back(argv[argIt]);
    }

    if(dict.is_open() && words.size() > 0)
    {
        Worker worker{dict};
        auto res = worker.search(words);

    }




    return 0;
}
