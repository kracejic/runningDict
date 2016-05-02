#pragma once
#include <fstream>
#include <string>

class Dict
{
  private:
    bool mIs_open{false};
    std::ifstream file;
    std::string mContents;
    std::string mFilename{""};
    bool mEnabled{true};

  public:
    Dict();
    Dict(std::string filename);
    Dict(std::string filename, int bonus, bool enabled);
    bool open(std::string filename);
    bool is_open();
    bool is_enabled();
    bool enable(bool state=true);
    bool toogle_enable();
    bool reload();
    const std::string &getFilename() const;
    const std::string &getContens() const;
    long long getContensSize() const;

    int mBonus{0};
};
