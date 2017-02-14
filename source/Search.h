#pragma once
#include <algorithm>
#include <cstdio>
#include <future>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include "Dict.h"
#include "Worker.h"


workerResult _search(std::vector<Dict>& dicts, int numthreads,
    const std::vector<std::string>& words, bool verbose = false);
