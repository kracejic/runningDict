#pragma once

#include <string>
#include <vector>

#include "Worker.h"



namespace Processer {
    /**
     * Returns list of small splitted up words
     * @param  text
     * @return
     */
    std::vector<std::string> splitToWords(char const* text);


    /**
     * Returns list of small splitted up words
     * @param  startIndex
     * @param  endIndex
     * @param  array
     * @return
     */
    std::vector<std::string> splitToWords(int startIndex, int endIndex,
                                          char const *array[]);
}
