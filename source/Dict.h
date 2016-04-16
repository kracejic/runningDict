#pragma once
#include <fstream>
#include <string>

class Dict
{
private:
    bool mIs_open {false};
    std::ifstream file;
    std::string mContents;
    std::string mFilename{""};

public:
    Dict();
    Dict(std::string filename);
    bool open(std::string filename);
    bool is_open();
    bool reload();
    const std::string& getFilename() const;
    const std::string& getContens() const;
};



