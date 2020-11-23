#include <iostream>
#include <ostream>
#include <istream>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>
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

cv::Mat grayScale(cv::Mat &regular, cl_context context, cl_command_queue queue)
{
    cv::Mat gray;
    gray.create(regular.size(), CV_8UC1);
    /*
    std::cout << "size: " << regular.size() << std::endl;
    std::cout << "cols: " << regular.cols << std::endl;
    std::cout << "rows: " << regular.rows << std::endl;
    std::cout << "rows*cols: " << regular.rows*regular.cols << std::endl;
    */
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



int main(int argc, char **argv){
    cv::setNumThreads(0);
    cv::VideoCapture inputVideo;
    char str_buffer[1024];
    if(!inputVideo.open(argv[1])){
    	std::cout << "error opening video" << std::endl;
	    exit(EXIT_FAILURE);
    }
    cl_int err;
    cl_platform_id platform ;
    cl_context context;
    cl_device_id device_id;
    cl_command_queue queue;
    err = clGetPlatformIDs(1, &platform, NULL);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
    err = clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(str_buffer), &str_buffer, NULL);
    printf("%s", str_buffer);
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    queue = clCreateCommandQueue (context, device_id, 0, &err);
   

    cv::Mat img;
    while(1){
        inputVideo >> img;

        if(img.empty()){
            std::cout << "last frame" << std::endl;
            break;
        }
        cv::Mat gray = grayScale(img, ker, queue);
        cv::imshow("gray", gray);
        cv::waitKey(33.36); // 29.97 fps


    }

    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    inputVideo.release();
    cv::destroyAllWindows();


   return 0;

}
