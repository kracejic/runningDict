#include "Dict.h"

using namespace std;

Dict::Dict()
{

}
Dict::Dict(string filename)
{
    mFilename = filename;
}
//-----------------------------------------------------------------------------------
const std::string& Dict::getFilename() const{
    return mFilename;
}
//-----------------------------------------------------------------------------------
bool Dict::reload()
{
    if(!is_open() && mFilename == "")
        return false;
    return (open(mFilename));
}
//-----------------------------------------------------------------------------------
bool Dict::open(std::string filename)
{
    if(is_open())
    {
        file.close();
        mContents.clear();
        mIs_open = false;
    }

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

