#include "model.h"
#include <iostream>
#include <future>
#include <string>
#include <fmt/format.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <vector>




void modeltest(int n)
{
    myModel<char> test;
    for(char i=0; i<n; i++)
    {
        test.push_back(i);
    }

    char & faiLTest = test[8192];
    faiLTest = 42;

    for(auto it  = test.Abegin(); it!=test.Aend(); it++)
    {
        if((*it)%2 ==0)
        {
            test.push_back(1);
            test.erase(it);
        }
    }
}
int main(int argc, char ** argv)
{
    if( argc<=2 )
    {
        fmt::print("Usage:{} <out file>", argv[0]);
        return -1;
    }
    std::string outputFileName = argv[1];
    char *x= new char[1000];


    delete []x;
    return 0; 
}