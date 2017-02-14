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

namespace Worker
{
workerResult search(const Dict& mDict, const std::vector<std::string>& wordsIn,
    long long start, long long end);
}
