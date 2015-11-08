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

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
struct deletable_facet : Facet
{
    template<class ...Args>
    deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};

int main(int argc, char const *argv[])
{
    cout<<"hello world!"<<endl;

    // wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> utfConvertor;
    wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>,
                    char16_t> utfConvertor;
    // wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> utfConvertor;

    // string x {"test"};
    // u32string y {"test"};
    // 
    #ifdef __MINGW32__
    cout<<"-----WINDOWS-----"<<endl;
    #endif



    for (int i = 1; i < argc; ++i)
    {
        cout<<"size="<<strlen(argv[i])<<endl;

        cout<<"  Input:"<<" :\""<<argv[i]<<"\" size="<<strlen(argv[i])<<endl;
        for (int ii = 0; ii < strlen(argv[i]); ++ii)
        {
            // cout<<"    "<<hex<<(unsigned int)argv[i][ii]<<endl;
            printf("    %02d = %ud %x\n", ii, (unsigned char)(argv[i][ii]), (unsigned char)(argv[i][ii]));
        }

        std::u16string a{utfConvertor.from_bytes(argv[i])};
        // for (int ii = 0; ii < strlen(argv[i]); ++ii)
        // {
        //     // a.append((char16_t) argv[i][ii]);
        //     // char16_t f = (int)((unsigned char)argv[i][ii]);
        //     a.push_back(argv[i][ii]);
        // }


        cout<<"  u16string:"<<endl;
        cout<<"    u16string.size()="<<a.size()<<endl;
        for (int ii = 0; ii < a.size(); ++ii)
        {
            cout<<"    "<<hex<<a[ii]<<endl;
        }

        string x = utfConvertor.to_bytes(a);
        cout<<"  Converted:"<<" :\""<<x<<"\" size="<<x.size()<<endl;
        for (int ii = 0; ii < x.size(); ++ii)
        {
            printf("    %02d = %d %x\n", ii, (unsigned char)(x[ii]), (unsigned char)(x[ii]));
        }
        cout<<endl;
    }



    // wstring a{argv[0]};

    // cout<<a<<endl;
    return 0;
}
