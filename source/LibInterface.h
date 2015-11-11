#pragma once
#include <vector>

#include "Dict.h"

extern std::vector<std::pair<int, Dict>> dicts;



extern "C" bool addDictionary(const char *filename, bool priority);

extern "C" char* search(const char* words);




extern "C" int test();







