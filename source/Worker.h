#pragma once
#include <vector>
#include <string>
#include <map>

#include "Dict.h"

struct Result
{
    Result(int sc, std::string st) : score(sc), words(st) {};
    int score;
    std::string words;
};

using workerResult = std::map<std::string,std::vector<Result>>;

class Worker
{
  private:
    long long mStart{0};
    long long mEnd{0};
    const Dict& mDict;
    int mBonus{0};




  public:
    Worker(Dict& dict, int bonus)
        : mDict(dict), mBonus(bonus){
            mEnd = dict.getContens().size();
        };
    workerResult search(const std::vector<std::string>& words, long long start,
                               long long end);
    workerResult search(const std::vector<std::string>& words);
};
