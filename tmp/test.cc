#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <locale>
#include <codecvt>

#include <iomanip>

//g++ test.cc  -std=c++11 -o a.exe && ./a.exe

using namespace std;

int main(int argc, char const *argv[])
{
    cout<<"hello world!"<<endl;
Z
    wstring_convert<codecvt_utf8<char16_t>, char16_t> utfConvertor;
    // wstring_convert<codecvt_utf8_utf16<char32_t>, char16_t> convertor2;

    // string x {"test"};
    // u32string y {"test"};



    for (int i = 0; i < argc; ++i)
    {
        cout<<"size="<<strlen(argv[i])<<" :\""<<argv[i]<<"\""<<endl;
        std::u16string a{utfConvertor.from_bytes(argv[i])};
        cout<<"  u16string.size()="<<a.size()<<endl;

        for (int ii = 0; ii < a.size(); ++ii)
        {
            cout<<"    "<<hex<<a[ii]<<endl;
        }
        cout<<"    "<<utfConvertor.to_bytes(a)<<endl;
    }



    // wstring a{argv[0]};

    // cout<<a<<endl;
    return 0;
}
