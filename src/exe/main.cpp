#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudawarping.hpp>
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

    for(auto& pair:cmd)
    {
        fmt::print("Command:{} Value: {} \n", pair.first, pair.second);
    }

    if(cmd.size()!=1)
    {
        DisplayHelp(cmd);
        return -1;
    }

    cv::Mat image = cv::imread(cmd["file"],1);
    auto imageSize = image.size();
    if(imageSize!=cv::Size(3840,2160))
    {
        fmt::print("Wrong image size {},{}. Should be 3840,2160", imageSize.width, imageSize.height);
    }
    if ( !image.data )
    {
        fmt::print("No image data \n");
        return -1;
    }
    // note that there issues with DPI settings...
    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
    cv::Mat tempResize;
    cv::resize(image,tempResize,cv::Size(1920,1080));
    cv::imshow("image", tempResize);
    cv::waitKey();

    cv::cuda::GpuMat gpuRes, tempBuf1, tempBuf2;
    tempBuf1.upload(image);
    tempBuf1.convertTo(gpuRes, CV_32FC3);

   
    static const float calibCameraMtx[3][3] ={{2.07794887e+03f,0.00000000e+00f,1.84670431e+03f},{0.00000000e+00f,2.08092007e+03f,9.68652791e+02f}, {0.00000000e+00f,0.00000000e+00f,1.00000000e+00f}};
    static const float distVector[5]={-3.96446557e-01f, 1.93910372e-01f,-2.63593840e-04f, 3.73261657e-05f,-4.91082365e-02f};
     
    cv::Mat tempMtx(cv::Size(3,3), CV_32F, (void*)calibCameraMtx[0]);
    cv::Mat tempVector(cv::Size(1,5), CV_32F, (void*)distVector);
    
    cv::Mat mapxCPU, mapyCPU;
    cv::Rect validPixels;
    cv::Mat optimalNewMtx = cv::getOptimalNewCameraMatrix(tempMtx, tempVector, imageSize, 0, imageSize, &validPixels);
    cv::initUndistortRectifyMap(
                tempMtx, tempVector, cv::Mat(),
                optimalNewMtx, imageSize,
                CV_32FC1, mapxCPU, mapyCPU);
    cv::cuda::GpuMat mapxCUDA, mapyCUDA;
    mapxCUDA.upload(mapxCPU);
    mapyCUDA.upload(mapyCPU);

    cv::cuda::remap(gpuRes,tempBuf1,mapxCUDA, mapyCUDA, cv::INTER_CUBIC);
    
    /*
    mtx = np.array([[2.07794887e+03,0.00000000e+00,1.84670431e+03], [0.00000000e+00,2.08092007e+03,9.68652791e+02], [0.00000000e+00,0.00000000e+00,1.00000000e+00]], np.float32)
    dist = np.array([[-3.96446557e-01, 1.93910372e-01,-2.63593840e-04, 3.73261657e-05,-4.91082365e-02]], np.float32)
    */
   /*
    cv::cuda::cvtColor(gpuRes,tempBuf2, cv::ColorConversionCodes::COLOR_RGB2GRAY);
 
    cv::Ptr<cv::cuda::CLAHE> ptr_clahe = cv::cuda::createCLAHE(5.0, cv::Size(8, 8));
    ptr_clahe->apply(tempBuf2, gpuRes);
    */
   
    cv::Mat result;
    tempBuf1.download(result);
    result.convertTo(result,CV_8UC3);
    cv::resize(result,result,cv::Size(1920,1080));
    cv::imshow("result", result);
    cv::waitKey();


}

void DisplayHelp(const CommandLine & cmd)
{
    fmt::print("Usage: {} --file=<fileName>\n", cmd.myExecutableName);
}