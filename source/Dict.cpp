#include "Dict.h"
#include <iostream>

using namespace std;

Dict::Dict()
{

}
Dict::Dict(string filename)
{
    mFilename = filename;
}
//------------------------------------------------------------------------------
Dict::Dict(std::string filename, int bonus, bool enabled)
    : mFilename(filename)
    , mEnabled(enabled)
    , mBonus(bonus)
{
    // std::cout<<"New Dict: filename = "<<filename<<std::endl;
    // std::cout<<"  bonus = "<<bonus<<std::endl;
    // std::cout<<"  enabled = "<<enabled<<std::endl;
    if(mEnabled)
        this->reload();
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
bool Dict::is_enabled()
{
    return mEnabled;
}
//------------------------------------------------------------------------------
bool Dict::toogle_enable()
{
    return this->enable(not mEnabled);
}
//------------------------------------------------------------------------------
bool Dict::enable(bool state)
{
    if (state == true)
    {
        mEnabled = true;
        if(mIs_open == false)
            mEnabled = reload();
    }
    else
        mEnabled = false;

    return mEnabled;
}
//------------------------------------------------------------------------------
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
    if(not mIs_open)
        throw std::domain_error{"Dictionary \"" + mFilename
                                + "\" was not loaded."};
    return mContents;
}
//-----------------------------------------------------------------------------------
long long Dict::getContensSize() const
{
    return mContents.size();
}
//------------------------------------------------------------------------------

