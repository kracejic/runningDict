#pragma once
#include <vector>



/**
 * Adds a dictionary if it was not added earlier
 * @param  filename filename of a dictionary
 * @param  priority priority
 * @return          true if found
 */
extern "C" bool addDictionary(const char *filename, bool priority);


/**
 * Conducts search on dictionaries which were added earlier
 * @param  words    string with space separated words, CamelCase will be parsed
 * @return          returns JSON
 */
extern "C" char* search(const char* words);








