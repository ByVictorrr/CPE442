#include <iostream>
#include <ostream>
#include <istream>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>

#include <CL/cl.hpp>
#include <CL/cl.h>

#define KERNEL_SOURCE "kernels.cl"

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
/*
std::string readKernel(const char *fileName){
    std::ifstream file(fileName);
     std::string content( (std::istreambuf_iterator<char>(file) ),
                       (std::istreambuf_iterator<char>()    ) );
    return content;
}

cv::Mat grayScale(cv::Mat &regular, cl_context context, cl_command_queue queue)
{
    cv::Mat gray;
    gray.create(regular.size(), CV_8UC1);
    /*
    std::cout << "size: " << regular.size() << std::endl;
    std::cout << "cols: " << regular.cols << std::endl;
    std::cout << "rows: " << regular.rows << std::endl;
    std::cout << "rows*cols: " << regular.rows*regular.cols << std::endl;
    * /
    std::string sourceGray = readKernel("gray_scale.cl");
    const char *_sourceGray = sourceGray.c_str();

    size_t globalThreads= regular.rows * regular.cols;
    cl_mem regularIMG, grayIMG;
    cl_int err;
    cl_program program = clCreateProgramWithSource(context, 1, &_sourceGray, NULL, &err);
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "gray_scale", &err);


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
    err = clEnqueueReadBuffer(queue, grayIMG, CL_TRUE, 0, (size_t)regular.rows*regular.cols, (void*)gray.ptr<uchar>(0), 0, NULL, NULL);
    return gray;
}

cv::Mat sobel(cv::Mat &regular, cl_context context, cl_command_queue queue){
    cv::Mat gray = grayScale(regular, context, queue);
    std::string sourceWP = readKernel("window_product.cl");
    const char *_sourceWP = sourceWP.c_str();
    size_t globalThreads= regular.rows * regular.cols;
    cl_program program;
    cl_kernel kernel; 
    cl_int err;

    cl_mem grayMEM;
    const cl_image_format format = {
        .image_channel_order=CL_R, 
        .image_channel_data_type=CL_UNSIGNED_INT8
    };
    const cl_image_desc = {
        .image_type=CL_MEM_OBJECT_IMAGE2D,
        .image_width=gray.cols,
        .image_height=gray.rows,
        .image_depth=0, // only for 3d images
        .image_array_size=0, // only if using type CL_MEM_OBJECT_IMAGE<2|1>D_ARRAY
        .image_row_pitch=,
        .image_slice_pitch=,
        .num_mip_levels=,
        .num_samples=,
        .buffer=
    };

    program = clCreateProgramWithSource(context, 1, &_sourceWP, NULL, &err);
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    kernel = clCreateKernel(program, "window_product", &err);

    // create images
    grayMEM = clCreateImage(context, CL_MEM_READ_ONLY, format, );


}
*/


int main(int argc, char **argv){
    //cv::setNumThreads(0);
    cv::VideoCapture inputVideo;
    cv::Mat img;
    
    //open and read first frame of input video to get matrix sizes
    if(!inputVideo.open(argv[1])){
    	std::cout << "error opening video" << std::endl;
	    exit(EXIT_FAILURE);
    }
    inputVideo >> img;  

    //opencl vars
    cl_int err;
    cl_platform_id platform ;
    cl_context context;
    cl_program program;
    cl_device_id device_id;
    cl_command_queue queue;
    char str_buffer[1024];
    size_t DATA_SIZE = sizeof(uchar) * img.cols * img.rows;

    err = clGetPlatformIDs(1, &platform, NULL);
    if (err!= CL_SUCCESS)
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
    printf("%s", str_buffer);
    // create a context with the GPU device
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    // create command queue using the context and device
    queue = clCreateCommandQueue (context, device_id, 0, &err);

    program = clCreateProgramWithSource(context, 1, (const char **) KERNEL_SOURCE, NULL, &err);
   //compile the program
    if (clBuildProgram(program, 0, NULL, NULL, NULL, NULL) != CL_SUCCESS)
    {
        printf("Error building program\n");
        exit(EXIT_FAILURE);
    }
    // select kernels from the program to execute
    cl_kernel gray_kernel = clCreateKernel(program, "gray_scale", &err);
    cl_kernel sobel_kernel = clCreateKernel(program, "sobel_product", &err);

    // create buffers for the input and ouput
    cl_mem input = clCreateBuffer(context, CL_MEM_READ_WRITE, DATA_SIZE * 3, NULL, NULL); //pixel of depth 3
    cl_mem gray_output = clCreateBuffer(context, CL_MEM_READ_WRITE, DATA_SIZE, NULL, NULL); //this is read/write to allow it to be reused without copying data
    cl_mem sobel_output = clCreateBuffer(context, CL_MEM_READ_WRITE, DATA_SIZE, NULL, NULL);

    while(1) {
        // load data into the input buffer
        err = clEnqueueWriteBuffer(queue, input, CL_TRUE, 0, DATA_SIZE * 3, img.data, 0, NULL, NULL);

        // set the argument list for the Grayscale kernel command
        clSetKernelArg(gray_kernel, 0, sizeof(cl_mem), &input);
        clSetKernelArg(gray_kernel, 1, sizeof(cl_mem), &gray_output);

        size_t global_work_size = DATA_SIZE;
        // enqueue the kernel command for execution
        clEnqueueNDRangeKernel(queue, gray_kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);

        clFinish(queue);

        uchar sobel_kernel_step = 3;
       // set the argument list for the sobel kernel command
        clSetKernelArg(sobel_kernel, 0, sizeof(cl_mem), &input);
        clSetKernelArg(sobel_kernel, 1, sizeof(cl_mem), &gray_output);
        clSetKernelArg(sobel_kernel, 2, sizeof(uchar), &sobel_kernel_step);
        clSetKernelArg(sobel_kernel, 3, sizeof(img.width), img.width);
        clSetKernelArg(sobel_kernel, 4, sizeof(img.height), img.height);
        // add more args here
        //perform sobel kernel
        global_work_size = DATA_SIZE / (sobel_kernel_step * sobel_kernel_step); //no need for 1 thread per pixel, only 1 per step^2 pixels
        clEnqueueNDRangeKernel(queue, sobel_kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);

        cv::Mat sobel;
        sobel.create(img.size(), CV_8UC1);
        
        // copy the results from out of the output buffer
        clEnqueueReadBuffer(queue, sobel_output, CL_TRUE, 0, DATA_SIZE, sobel.data, 0, NULL, NULL);
        
        clFinish(queue);

        cv::imshow("sobel", sobel);
        cv::waitKey(33.36); // 29.97 fps

        //q next frame
        image >> img;
        if(img.empty()){
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
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context)
    inputVideo.release();
    cv::destroyAllWindows();


   return 0;

}
