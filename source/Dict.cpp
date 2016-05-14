#include "Dict.h"
#include <iostream>
#include <algorithm>

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
    //presume problems, reseted later
    mErrorState = true;

    //if open, close
    if(is_open())
    {
        file.close();
        mContents.clear();
        mIs_open = false;
    }

    //reopen
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
    mErrorState = false;



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
bool Dict::addWord(const std::string& word, const std::string& translation)
{
    if(not mIs_open)
        this->open(mFilename);
    if(not mIs_open)
        return false;

    //TODO make word lower case

    //erase whitespace on the end of mContens
    mContents.erase(std::find_if(mContents.rbegin(), mContents.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace)))
                .base(), mContents.end());

    //add word
    mContents.append("\n"s+word+"\n "+translation);

    this->saveDictionary();

    return true;
}
//------------------------------------------------------------------------------
void Dict::saveDictionary()
{
    std::ofstream outfile{mFilename};
    outfile<<mContents<<endl;
}
//------------------------------------------------------------------------------


