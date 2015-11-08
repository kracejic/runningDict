#pragma once
#include <vector>
#include <string>
#include <map>

#include "Dict.h"

struct Result
{
    Result(float sc, std::string st) : score(sc), words(st) {};
    float score;
    std::string words;
};


class Worker
{
  private:
    long long mPosition{0};
    long long mStart{0};
    long long mEnd{0};
    const Dict& mDict;




  public:
    Worker(Dict& dict)
        : mDict(dict){
            mEnd = dict.getContens().size();
        };
    std::map<std::string,std::vector<Result>> search(const std::vector<std::string>& words, long long start,
                               long long end);
    std::map<std::string,std::vector<Result>> search(const std::vector<std::string>& words);
};
