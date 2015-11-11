#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <cstdio>
#include <future>

#include "Dict.h"
#include "Worker.h"


workerResult _search(std::vector<std::pair<int, Dict>>& dicts,
                     int numthreads, const std::vector<std::string>& words,
                     bool verbose);
