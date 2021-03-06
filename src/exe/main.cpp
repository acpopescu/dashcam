#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudawarping.hpp>
#include <future>
#include "utils/CommandLine.h"
#include <fmt/format.h>
void DisplayHelp(const CommandLine & cmd);
int main(int argc, char** argv) 
{
    const CommandLine cmd(argc, argv);
    std::cout << "Hello, world!\n";
    fmt::print("And now using FMT! Here are the command lines \n");
    int devcount = cv::cuda::getCudaEnabledDeviceCount();
    fmt::print("CUDA enabled devices: {}", devcount);
    if(devcount<1)
    {
        fmt::print("this program requires CUDA, exiting\n");
        return -1;
    }
    cv::cuda::printCudaDeviceInfo(0);
    cv::cuda::setDevice(0);

    if(cmd.size()!=2)
    {
        DisplayHelp(cmd);
        return -1;
    }

    cv::VideoCapture cap(cmd["in"], cv::CAP_FFMPEG);
    if(!cap.isOpened())
    {
        fmt::print("Unable to open for input {}. Exiting.", cmd["in"]);
        return -1;
    }
    cv::Size refS = cv::Size((int) cap.get(cv::CAP_PROP_FRAME_WIDTH),
                     (int) cap.get(cv::CAP_PROP_FRAME_HEIGHT));

    if(refS!=cv::Size(3840,2160))
    {
        fmt::print("Wrong image size {},{}. Should be 3840,2160 for this b4k utility", refS.width, refS.height);
    }

    int ex = static_cast<int>(cap.get(cv::CAP_PROP_FOURCC));
    
    cv::VideoWriter outVideo(cmd["out"],
        
        ex,
        cap.get(cv::CAP_PROP_FPS), refS, true);
    
    if(!outVideo.isOpened())
    {
        fmt::print("Unable to open for output {}. Exiting.", cmd["out"]);
        return -1;
    }

    // configure remapping parameters and upload to GPU
    static const float calibCameraMtx[3][3] ={{2.07794887e+03f,0.00000000e+00f,1.84670431e+03f},{0.00000000e+00f,2.08092007e+03f,9.68652791e+02f}, {0.00000000e+00f,0.00000000e+00f,1.00000000e+00f}};
    static const float distVector[5]={-3.96446557e-01f, 1.93910372e-01f,-2.63593840e-04f, 3.73261657e-05f,-4.91082365e-02f};
     
    cv::Mat tempMtx(cv::Size(3,3), CV_32F, (void*)calibCameraMtx[0]);
    cv::Mat tempVector(cv::Size(1,5), CV_32F, (void*)distVector);
    
    cv::Mat mapxCPU, mapyCPU;
    cv::Rect validPixels;
    cv::Mat optimalNewMtx = cv::getOptimalNewCameraMatrix(tempMtx, tempVector, refS, 0, refS, &validPixels);
    cv::initUndistortRectifyMap(
                tempMtx, tempVector, cv::Mat(),
                optimalNewMtx, refS,
                CV_32FC1, mapxCPU, mapyCPU);
    cv::cuda::GpuMat mapxCUDA, mapyCUDA;
    mapxCUDA.upload(mapxCPU);
    mapyCUDA.upload(mapyCPU);

    // start converting video
    cv::Mat frameIn;
    cv::Mat cpuPreview;

    // needed volatile buffers. 
    cv::cuda::GpuMat stagingFrameUpload, gpuFrameFloat;
    cv::cuda::GpuMat remapOut, previewResize, previewStaging;
    cv::cuda::GpuMat outputStaging;


    // note that there issues with DPI settings on the openCV GUI
    cv::namedWindow("Preview", cv::WINDOW_AUTOSIZE);
    std::future<void> imgdisp, imgdecode, avwrite, avstaging;

    // this pipeline is made to work in parallel
    // decode, image display processing and encode are in parallel

    // launch first frame decode.
    imgdecode = std::async(std::launch::async,[&]()
    {
        cap >> frameIn;        
        if(frameIn.empty())
        {
            return;
        }
        // re-use remapOut as staging
        stagingFrameUpload.upload(frameIn);
        stagingFrameUpload.convertTo(gpuFrameFloat, CV_32FC3);
    });
    while(1)
    {
        // wait for preview and display
        if(imgdisp.valid())
        {
            imgdisp.wait();
            cv::imshow("Preview", cpuPreview);
            cv::waitKey(2);
        }
        // wait for decode
        if(imgdecode.valid())
        {
            imgdecode.wait();
        }
        
        // we only need to wait for staging here.
        if(avstaging.valid())
        {
            avstaging.wait();
        }
        if(frameIn.empty())
        {
            break;
        }
       
        cv::cuda::remap(gpuFrameFloat, remapOut,mapxCUDA, mapyCUDA, cv::INTER_CUBIC);
        cv::cuda::resize(remapOut, previewResize,cv::Size(1920,1080));

        imgdisp = std::async(std::launch::async, 
        [&](){
            previewResize.convertTo(previewStaging, CV_8UC3);
            previewStaging.download(cpuPreview);
        });
        imgdecode = std::async(std::launch::async,[&]()
        {
            cap >> frameIn;        
            if(frameIn.empty())
            {
                return;
            }
            stagingFrameUpload.upload(frameIn);
            stagingFrameUpload.convertTo(gpuFrameFloat, CV_32FC3);
        });

        // wait for avwrite
        if(avwrite.valid())
        {
            avwrite.wait();
        }
        avstaging = std::async(std::launch::async, 
        [&](){
            remapOut.convertTo(outputStaging, CV_8UC3);
        });
        avwrite = std::async(std::launch::async, 
        [&](){
            avstaging.wait();
            cv::Mat frameOut;
            outputStaging.download(frameOut);
            outVideo << frameOut;
        });        
    }
}

void DisplayHelp(const CommandLine & cmd)
{
    fmt::print("Usage: {} --in=<videoIn> --out=<videoOut>\n", cmd.myExecutableName);
}