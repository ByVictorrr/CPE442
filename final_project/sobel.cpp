#define CL_TARGET_OPENCL_VERSION 120

#include <iostream>
#include <ostream>
#include <istream>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>
#include <CL/cl.h>

std::string readKernel(const char *fileName)
{
    std::ifstream file(fileName);
    std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    return content;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "sobel <video_file> " << std::endl;
        exit(EXIT_FAILURE);
    }
    //cv::setNumThreads(0);
    cv::VideoCapture inputVideo;
    cv::Mat img;

    //open and read first frame of input video to get matrix sizes
    if (!inputVideo.open(argv[1]))
    {
        std::cout << "error opening video" << std::endl;
        exit(EXIT_FAILURE);
    }
    inputVideo >> img;

    //opencl vars
    cl_int err;
    cl_platform_id platform;
    cl_context context;
    cl_program program;
    cl_device_id device_id;
    cl_command_queue queue;
    char str_buffer[1024];
    size_t DATA_SIZE = sizeof(unsigned char) * img.cols * img.rows;

    err = clGetPlatformIDs(1, &platform, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Unable to get platform_id\n");
        exit(EXIT_FAILURE);
    }
    // try to get a supported GPU device
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Unable to get device_id\n");
        exit(EXIT_FAILURE);
    }
    //if device found, print info
    err = clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(str_buffer), &str_buffer, NULL);
    printf("%s\n", str_buffer);
    // create a context with the GPU device
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    // create command queue using the context and device
    queue = clCreateCommandQueue(context, device_id, 0, &err);

    std::string kernel_source = readKernel("kernels.cl");
    const char *_kernel_source = kernel_source.c_str();
    program = clCreateProgramWithSource(context, 1, &_kernel_source, NULL, &err);
    //compile the program
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        //clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 1024, str_buffer, NULL);
        //printf("Error building program: %d : %s\n", err, str_buffer);
        printf("Error building program: %d\n", err);
        exit(EXIT_FAILURE);
    }
    // select kernels from the program to execute
    cl_kernel gray_kernel = clCreateKernel(program, "Gray", &err);
    if (err != CL_SUCCESS)
    {
        printf("gray kernel failed: %d\n", err);
        exit(EXIT_FAILURE);
    }
    cl_kernel sobel_kernel = clCreateKernel(program, "Sobel", &err);
    if (err != CL_SUCCESS)
    {
        printf("sobel kernel failed: %d\n", err);
        exit(EXIT_FAILURE);
    }

    // create buffers for the input and ouput
    cl_mem input = clCreateBuffer(context, CL_MEM_READ_WRITE, DATA_SIZE * 3, NULL, NULL);   //pixel of depth 3
    cl_mem gray_output = clCreateBuffer(context, CL_MEM_READ_WRITE, DATA_SIZE, NULL, NULL); //this is read/write to allow it to be reused without copying data
    cl_mem sobel_output = clCreateBuffer(context, CL_MEM_READ_WRITE, DATA_SIZE, NULL, NULL);

    while (1)
    {
        // load data into the input buffer
        err = clEnqueueWriteBuffer(queue, input, CL_TRUE, 0, DATA_SIZE * 3, img.data, 0, NULL, NULL); //img.ptr<uchar>(0)

        // set the argument list for the Grayscale kernel command
        clSetKernelArg(gray_kernel, 0, sizeof(cl_mem), &input);
        clSetKernelArg(gray_kernel, 1, sizeof(cl_mem), &gray_output);

        size_t global_work_size = DATA_SIZE;
        //DEBUG std::cout << "img rows: " << img.rows << " img cols: " << img.cols << " work size: " << global_work_size << std::endl;
        // enqueue the kernel command for execution
        err = clEnqueueNDRangeKernel(queue, gray_kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);
        if (err != CL_SUCCESS)
        {
            printf("gray kernel failed: %d\n", err);
            exit(EXIT_FAILURE);
        }
        err = clFinish(queue);

        unsigned int step = 3;
        unsigned int width = img.cols;
        unsigned int height = img.rows;
        // set the argument list for the sobel kernel command
        clSetKernelArg(sobel_kernel, 0, sizeof(cl_mem), &gray_output);
        clSetKernelArg(sobel_kernel, 1, sizeof(cl_mem), &sobel_output);
        clSetKernelArg(sobel_kernel, 2, sizeof(unsigned int), &step);
        clSetKernelArg(sobel_kernel, 3, sizeof(unsigned int), &width);
        clSetKernelArg(sobel_kernel, 4, sizeof(unsigned int), &height);

        //perform sobel kernel
        const unsigned int sobel_work_size[2] = {width, height}; //x,y
        err = clEnqueueNDRangeKernel(queue, sobel_kernel, 2, NULL, sobel_work_size, NULL, 0, NULL, NULL);
        if (err != CL_SUCCESS)
        {
            printf("sobel kernel failed: %d\n", err);
            exit(EXIT_FAILURE);
        }

        cv::Mat sobel(img.size(), CV_8UC1);

        // copy the results from out of the output buffer
        err = clEnqueueReadBuffer(queue, sobel_output, CL_TRUE, 0, DATA_SIZE, sobel.data, 0, NULL, NULL); //sobel_output
        err = clFinish(queue);

        cv::imshow("sobel", sobel);
        cv::waitKey(33.36); // 29.97 fps
        //DEBUG cv::waitKey(0);

        //q next frame
        inputVideo >> img;
        if (img.empty())
        {
            std::cout << "last frame" << std::endl;
            break;
        }
    }

    // cleanup - release OpenCL resources
    clReleaseMemObject(input);
    clReleaseMemObject(gray_output);
    clReleaseMemObject(sobel_output);
    clReleaseProgram(program);
    clReleaseKernel(gray_kernel);
    clReleaseKernel(sobel_kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    inputVideo.release();
    cv::destroyAllWindows();

    return 0;
}
