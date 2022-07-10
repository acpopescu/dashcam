#include "model.h"
#include <iostream>
#include <future>
#include <string>
#include <fmt/format.h>
#include <vector>
#include "utils/CommandLine.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}



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

void printFFMPEGError(int result, const std::string & inFileName)
{
    char avErrorBuffer[AV_ERROR_MAX_STRING_SIZE+1] = {0};
    av_strerror(result,avErrorBuffer,AV_ERROR_MAX_STRING_SIZE);    
    fmt::print("Error processing {}:{}\n", inFileName, avErrorBuffer);
}

int main(int argc, char ** argv)
{
    const CommandLine cmd(argc, argv);
    if( argc<=2 || cmd["in"]=="")
    {
        fmt::print("Usage:{} --in=<in file>", argv[0]);
        return -1;
    }
    std::string inFileName = cmd["in"];
    
    AVFormatContext * inAVFormatContext = avformat_alloc_context();
    int result = avformat_open_input(&inAVFormatContext, inFileName.c_str(), nullptr, nullptr);
    if(result<0)
    {
        printFFMPEGError(result, inFileName);
        return result;
    }
    
    result = avformat_find_stream_info(inAVFormatContext, nullptr);
    if(result<0)
    {
        printFFMPEGError(result, inFileName);
        return result;
    }

    fmt::print("File format : {}. Stream count: {}. Duration microseconds: {}\n", 
        inAVFormatContext->iformat->long_name, 
        inAVFormatContext->nb_streams,
        inAVFormatContext->duration);

    for(uint32_t streamIdx=0; streamIdx < inAVFormatContext->nb_streams; streamIdx++)
    {
        AVStream * stream =  inAVFormatContext->streams[streamIdx];
        AVCodecParameters * codecParams = stream->codecpar;
        AVCodec * codec = avcodec_find_decoder(codecParams->codec_id);
        if(codec)
        {
            switch(codec->type)
            {
                case AVMEDIA_TYPE_AUDIO:
                    fmt::print("{:>2} {:>8} {:>8}bps {:>4}x{:<6}\n", streamIdx, codec->name, codecParams->bit_rate, codecParams->channels, codecParams->sample_rate);
                    break;
                case AVMEDIA_TYPE_VIDEO:
                    fmt::print("{:>2} {:>8} {:>8}bps {:>4}x{:<6}\n", streamIdx, codec->name, codecParams->bit_rate, codecParams->width, codecParams->height);
                    break;
                default:
                    fmt::print("{:>2} {:>8} {:>8}bps {:>4}x{:<6}\n", streamIdx, codec->name, codecParams->bit_rate, 0, 0);
                    break;
            }
            
        }
        else
        {
            fmt::print("{:>2} {:>8} {>:8}bps {:>4}x{:<6}\n", streamIdx, "null", 0, 0, 0);
        }
    }
    return 0; 
}