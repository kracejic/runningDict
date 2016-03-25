#pragma once

#include <string>
#include <vector>

#include "Worker.h"




class Word
{
public:
    std::string text;
    Word(const std::string& txt);

    std::vector<std::string> words1;
    std::vector<std::string> words2;
    std::vector<std::string> words3;

    std::vector<std::string> getAllWordsBig();
    std::vector<std::string> getAllWordsSmall();
};


class Processer
{
private:
    std::vector<Word> words;

public:
    Processer(char const* text);
    Processer(int argstart, int argc, char const* argv[]);
    std::vector<std::string> getAllWordsBig();
    std::vector<std::string> getAllWordsSmall();

    void process (const workerResult& results);
};



