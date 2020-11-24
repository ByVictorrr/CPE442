# CPE 442 : Final Project : GPU Accelerated Sobel Filter

# Author(s)
* Victor Delaplaine
* Tristan Chutka

# Video link
* [Video demonstrating the program with GPU acceleration.](https://youtu.be/i1DkAqWu_ic)

# Timing Table
|                 | Single | Multithreaded |    GPU    |
|-----------------|--------|---------------|-----------|
|average SPF |  2.182  |     1.523      |   1.278   |

Since the standard unit of video (frames per second) is not applicable when dealing with the incredibly under-powered Pi 3, seconds per frame (SPF) was used instead. Time was measured from the import of the Mat object to the display of the filtered image. For the multithreaded program, the average time of all threads was measured and divided by the number of buffered frames. 

## Optimizations
This program is an even further improvement upon the last assignment(s), as it impliments GPGPU encoding for the Sobel and Grayscale kernerls. 

The Pi 3 has a [VideoCore IV Graphics coproccessor](https://en.wikipedia.org/wiki/VideoCore#Table_of_SoCs_adopting_VideoCore_SIP_blocks) with roughly 24GFLOPS of compute power, not excedingly fast, but enough to see marginal performance gains. The Grayscale and Sobel shader cores are created in OpenCL, support for which is integrated into OpenCV (through the TransparentAPI OpenCV-OpenCL integration utilities), and the [VC4C compiler](https://github.com/doe300/VC4C) providing partial OpenCL 1.2 support on the RPi VideoCore IV. 

### What is OpenCL?
* Open Computing Language (OpenCL) is a portable framework that executes across different devices such as central processing untis (CPUs), graphics procecssing units (GPUs), digital signal processors(DSPs), field-programmable gate arrays (FPGAs) and other processors or hardware accelerators. 
* It specifies its language in C99 and C++11 for programing these devices via API's to control and execute programs on the compute devices.
* It provides a standard interface for parallel computing by using task- and data-based paralleism.

#### OpenCL Architecture
* OpenCL has a certain architecture to it in order to accomplish running code on differnt types of devices such as a GPU, FPGA, ect.
* Different abstractions involve:
    1. Platform
    2. Context
    3. Device
    4. Kernels
    5. Buffers
    6. CommandQueue

![overview](https://i.imgur.com/tMIsjbs.png)

##### Platform
* A platform defines a specific instance of OpenCL software installed on your computer

[cl_int clGetPlatformIDs(cl_uint num_entries,cl_platform_id *platforms,cl_uint *num_platforms](https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clGetPlatformIDs.html)

    * The above function is used to return a list of platforms installed on your computer

![platform model](https://i.imgur.com/GsE0M9B.png)

##### Context
* This is used so that differnt OpenCL Devices would have the ability to share memory.
* Its also a way to link queues, and buffers to the OpenCL Devices.

[cl_context clCreateContext(cl_context_properties *properties,cl_uint num_devices,const cl_device_id *devices,void *pfn_notify (const char *errinfo ,,const void *private_info,size_t cb, void *user_data),void *user_data,cl_int *errcode_ret)](https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clCreateContext.html)

##### Device
* OpenCL uses differnt ways to describe devices, it could be the CPU, GPU, ect.
* The host CPU can be looked as two things: 
    1. Host device
    2. OpenCL device

* You feed each device a command queue and buffers via the context

[cl_int clGetDeviceIDs(	cl_platform_id platform, cl_device_type device_type,cl_uint num_entries,cl_device_id *devices,cl_uint *num_devices)](https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clGetDeviceIDs.html)

##### Programs
* This is where you read in your source code for your kernel and compile it via the OpenCL compiler
[cl_program clCreateProgramWithSource (	cl_context context,cl_uint count,const char **strings,const size_t *lengths,cl_int *errcode_ret)](https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clCreateProgramWithSource.html)

##### Kernels
A kernel is a function written in OpenCL framework that executable on the computing devices. The number of threads to be spawned is known as a workgroup, and can be broken into several factors:
    * The global size, which is the total number of work-items
    * The local size, which is the number of work-items per work-group
    * The number of work-groups is the global size / local size
    
There is no limit on the number of threads that can be spawned. For our application, the kernels launch 1 thread per pixel, with the grayscale only utilizing 1 dimension (total thread count of rows * cols), while the sobel kernel utilizes 2 dimensions for ease of addressing (cols is the number of threads in dimension 1, or x, and rows the number in dimension 2, or y)

You can seperate this function to be run on different space 1D,2D,3D.
* For Example, with a 100x100 image
    * I could seperate run 1 thread for each image
    * get_global_id(dim): gets the global id of each work-item
        * dim=0 (x-direction)
        * dim=1 (y-direction)
        * dim=2 (z-direction)
    * Each thread would be assigned a x and y via get_global_id and work on individual pixels. In this example 100000 threads would be spawned, 100 in dimension x, and 100 in dimension y. 
Also you use the kernel object to set the parameters for your kernel

[cl_kernel clCreateKernel(cl_program program,const char* kernel_name,cl_int* errcode_ret)](https://www.khronos.org/registry/OpenCL/sdk/2.2/docs/man/html/clCreateKernel.html)

**Note: A kernel executable code, the Program object is the one that compiles it and sends it to the context**

###### Buffers
* This is a way for us to get/put data into our devices, by putting writing/reading commands int our queue from the buffer
* Usually these buffers are created for the arguments of the kernel.
* Some likely to be used buffers are images/buffers

`Create Buffer`
[cl_mem clCreateBuffer (cl_context context,cl_mem_flags flags, size_t size,void *host_ptr,cl_int *errcode_ret)](https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clCreateBuffer.html)

Address spaces can be prefixed with scope declarations:
* `constant` A small portion of cached global memory visible by all workers. Use it if you can, read only.
* `global` Slow, visible by all, read or write. It is where all your data will end, so some accesses to it are always necessary.
* `local` Do you need to share something in a local group? Use local! Do all your local workers access the same global memory? Use local! Local memory is only visible inside local workers, and is limited in size, however is very fast.
* `private` Memory that is only visible to a worker, consider it like registers. All non defined values are private by default.

`Create Image`
[cl_mem clCreateImage (	cl_context context,cl_mem_flags flags,const cl_image_format *image_format,const cl_image_desc *image_desc,void *host_ptr,cl_int *errcode_ret)](https://www.khronos.org/registry/OpenCL/sdk/1.2/docs/man/xhtml/clCreateImage.html)


###### Command Queue
* This is a way to feed the device command.
* For example say we want to write/read from a buffer in our device
    * We can put the commands in the queue and specifiy the host_ptr where to store/load from/to

`Create Command Queue`
[cl_command_queue clCreateCommandQueue(	cl_context context,cl_device_id device,cl_command_queue_properties properties,cl_int *errcode_ret)](https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clCreateCommandQueue.html)

`Writing to the buffer`
[cl_int clEnqueueWriteBuffer (	cl_command_queue command_queue,cl_mem buffer,cl_bool blocking_write,size_t offset,size_t cb,const void *ptr, cl_uint num_events_in_wait_list, const cl_event *event_wait_list,cl_event *event)](https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clEnqueueWriteBuffer.html)

`Reading from the buffer`
[cl_int clEnqueueReadBuffer (	cl_command_queue command_queue,cl_mem buffer,cl_bool blocking_read,size_t offset,size_t cb,void *ptr,cl_uint num_events_in_wait_list,const cl_event *event_wait_list,cl_event *event)](https://www.khronos.org/registry/OpenCL/sdk/1.0/docs/man/xhtml/clEnqueueReadBuffer.html)




### Installation
#### Dependencies
```bash
$ sudo apt-get update
$ sudo apt-get upgrade

$ sudo apt-get install cmake git
$ sudo apt-get install ocl-icd-opencl-dev ocl-icd-dev
$ sudo apt-get install opencl-headers
$ sudo apt-get install clinfo
$ sudo apt-get install libraspberrypi-dev

$ sudo apt-get install clang clang-format clang-tidy
```

#### Build Library from source
```bash
$ mkdir -p ~/opencl
$ cd ~/opencl
$ git clone https://github.com/doe300/VC4CLStdLib.git
$ git clone https://github.com/doe300/VC4CL.git
$ git clone https://github.com/doe300/VC4C.git

$ cd ~/opencl/VC4CLStdLib
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
$ sudo ldconfig

$ cd ~/opencl/VC4C
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
$ sudo ldconfig

$ cd ~/opencl/VC4CL
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
$ sudo ldconfig
```

### Utilization
VC4C targets OpenCL 1.2. To define this in a program, inlucde `#define CL_TARGET_OPENCL_VERSION 120` at the begining of the source file (note that this line must be found *before* the `#include CL/cl.h`).

Syntax is standard OpenCL 1.2, with work size equal to the total pixel count for both kernels, with the sobel kernel using a 2D work size depth (allowing for easy retrieval of x and y coordinates).

Other things to note: the first input image is RGB, and thus has a spacing of 3 `unsigned char`s per pixel. GPU-side allocated memory must take this into account. 