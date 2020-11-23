#include <iostream>
#include <ostream>
#include <istream>
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/ocl.hpp"
#include <arm_neon.h>

#include <CL/cl.hpp>
#include <CL/cl.h>

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
std::string readKernel(const char *fileName){
    std::ifstream file(fileName);
     std::string content( (std::istreambuf_iterator<char>(file) ),
                       (std::istreambuf_iterator<char>()    ) );
    return content;
}

cv::Mat grayScale(cv::Mat &regular)
{
    cv::Mat gray;
    std::cout << "size: " << regular.size() << std::endl;
    std::cout << "cols: " << regular.cols << std::endl;
    std::cout << "rows: " << regular.rows << std::endl;
    std::cout << "rows*cols: " << regular.rows*regular.cols << std::endl;
    gray.create(regular.size(), CV_8UC1);
    size_t szParmDataBytes; // for number of gpus
    cl_platform_id platform ;
    cl_context context;
    cl_device_id device_id;
    cl_kernel kernel;
    cl_command_queue queue;
    cl_mem regularIMG, grayIMG;
    cl_int err;
    std::string sourceKernal = readKernel("gray_scale.cl");
    const char *_sourceKernel = sourceKernal.c_str();

    size_t globalThreads= regular.rows * regular.cols;
    // Create the openCL context on a GPU device
    err = clGetPlatformIDs(1, &platform, NULL);
    // Get the list of GPU devices associated with context
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    // Create a command-queue
    queue = clCreateCommandQueue (context, device_id, 0, &err);


    // Create the program and load the kernel source to it
    cl_program program = clCreateProgramWithSource(context, 1, &_sourceKernel, NULL, &err);
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    kernel = clCreateKernel(program, "gray_scale", &err);

    // allocate the first buffer (src)
    regularIMG = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(uchar) * regular.cols * regular.rows * 3, NULL, &err);
    grayIMG = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(uchar)* regular.cols * regular.rows, NULL, &err) ;
    // Write our data set into the input array in device memory
    err = clEnqueueWriteBuffer(queue, regularIMG, CL_TRUE, 0, regular.cols*regular.rows*3, regular.data, 0, NULL, NULL);
     
    // setting kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&regularIMG);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&grayIMG);
    // Enqueue a command to execute a kernel on device
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalThreads, NULL, 0, NULL, NULL);
    err = clFinish(queue);
    std::cout << "finished" << std::endl;

    // read back the result
    err = clEnqueueReadBuffer(queue, grayIMG, CL_TRUE, 0, (size_t)regular.rows*regular.cols, (void*)gray.data, 0, NULL, NULL);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return gray;
}



int main(int argc, char **argv){
    //cv::setNumThreads(0);
    cv::VideoCapture inputVideo;
    cv::Mat img;
    inputVideo.open(argv[1]);

    while(1){
        inputVideo >> img;
        cv::Mat gray = grayScale(img);

        /*
        for(int i = 0; i<img.rows*img.cols;i++)
            std::cout << std::to_string(gray.data[i]) << std::endl;
            */

        cv::imshow("gray", gray);
        cv::waitKey(3);
        
        


    }

   return 0;

}