#pragma once
#include <fstream>
#include <string>

class Dict
{
private:
    bool mIs_open {false};
    std::ifstream file;
    std::string mContents;

public:
    Dict();
    bool open(std::string filename);
    bool is_open();
    const std::string& getContens() const;
};



