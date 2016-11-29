#pragma once
#include <map>
#include <string>
#include <vector>

#include "Dict.h"

struct Result
{
    Result(
        int sc, std::string st, std::string _match, std::string _dictFilename)
        : score(sc)
        , words(st)
        , match(_match)
        , dictFilename(_dictFilename){};
    int score;
    std::string words;
    std::string match;
    std::string dictFilename;
};

using workerResult = std::map<std::string, std::vector<Result>>;

class Worker
{
  private:
    long long mStart{0};
    long long mEnd{0};
    const Dict& mDict;
    int mBonus{0};


  public:
    Worker(Dict& dict)
        : mDict(dict)
        , mBonus(dict.mBonus)
    {
        mEnd = dict.getContens().size();
    };
    workerResult search(
        const std::vector<std::string>& words, long long start, long long end);
    workerResult search(const std::vector<std::string>& words);
};
