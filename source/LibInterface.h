#pragma once


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
 * @return          returns JSON in a form of char*. There is no need to free
 *                  the memory since it will be freed on next query. Parallel 
 *                  queries are not yet possible.
 */
extern "C" char* search(const char* words);



/**
 * set number of threads used for searching
 * @param numThreads 
 */
extern "C" void setNumberOfThreads(int numThreads);






