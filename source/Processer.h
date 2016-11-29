#pragma once

#include <string>
#include <vector>

#include "Worker.h"


namespace Processer
{
/**
 * Returns list of small splitted up words
 * @param  text
 * @return
 */
std::vector<std::string> splitToWords(char const* text);


/**
 * Returns list of small splitted up words. Used for main function which
 * processes argc and argv
 * @param  startIndex current Arg index
 * @param  endIndex number of Args (argc)
 * @param  array argv
 * @return
 */
std::vector<std::string> splitToWords(
    int startIndex, int endIndex, char const* array[]);
}
