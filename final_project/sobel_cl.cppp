#include <iostream>
#include <ostream>
#include <istream>
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/ocl.hpp"
#include "threadhandler.h"
#include <arm_neon.h>

#include <CL/cl.hpp>

#include <linux/perf_event.h>

#define KERNELS "kernels.cl"

class ArgParser{
    public:
        static const char *parse(int argc, char**argv){
            if (argc != 2)
            {
                std::cerr << "sobel <video_file> " << std::endl;
                exit(EXIT_FAILURE);
            }
            return argv[1];
        }
};

// TODO: input <clinfo>
uchar *getData(const cv::Mat &img){
    return img.data;
}



int main(int argc, char **argv){
    cv::VideoCapture inputVideo;
    cv::Mat img;
    cv::ocl::Context context;
    // Step 1 - check to see if opencl integration is there
    if(!cv::ocl::haveOpenCL()){
        std::cerr << "OpenCL is not available" << std::endl;
        exit(EXIT_FAILURE);
    }
    else if(!inputVideo.open(ArgParser::parse(argc, argv))){
        std::cerr << "Not able to open: " << argv[1] << std::endl;
        exit(EXIT_FAILURE);
    }
    else if(!context.create(cv::ocl::Device::TYPE_GPU)){
        std::cerr << "Failed creating the context" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << context.ndevices() << " GPU devices detected." << std::endl;
    // Select the first device
    cv::ocl::Device gpu(context.device(0));

    while(1){
        inputVideo >> img;
        cv::UMat uimg = img.getUMat(cv::ACCESS_READ, cv::USAGE_ALLOCATE_DEVICE_MEMORY);
        cv::UMat ugray(img.size(), CV_8S, cv::ACCESS_WRITE, cv::USAGE_ALLOCATE_DEVICE_MEMORY);
        std::ifstream ifs(KERNELS);
        if(ifs.fail()){
            exit(EXIT_FAILURE);
        }
        std::string source((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        cv::ocl::ProgramSource programSource(source);

        // Compile the kernel code
        cv::String errmsg;
        cv::String buildopt = cv::format("-D dstT=%s", cv::ocl::typeToStr(ugray.depth())); // "-D dstT=float"
        cv::ocl::Program program = context.getProg(programSource, buildopt, errmsg);
        
        cv::ocl::Image2D imageCL(uimg);
        cv::ocl::Kernel kernel("gray_scale", program);
        kernel.args(imageCL); // TODO put other args in

        
        
        


    }

   return 0;

}