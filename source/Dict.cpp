#include "Dict.h"

using namespace std;

Dict::Dict()
{

}
//-----------------------------------------------------------------------------------
const std::string& Dict::getFilename() const{
    return mFilename;
}
//-----------------------------------------------------------------------------------
bool Dict::open(std::string filename)
{
    file.open(filename);

    if (!file.is_open())
        return false;
    mFilename = filename;

    file.seekg(0, std::ios::end);
    mContents.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    mContents.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    mIs_open = true;



    return true;
}
//-----------------------------------------------------------------------------------
bool Dict::is_open()
{
    return mIs_open;
}
//-----------------------------------------------------------------------------------
const std::string& Dict::getContens() const{
    return mContents;
}
//-----------------------------------------------------------------------------------

