# CPE 442 : Final Project : GPU Accelerated Sobel Filter

# Author(s)
* Victor Delaplaine
* Tristan Chutka

# Video link
* [Video demonstrating the program with GPU acceleration.](https://youtu.be/g0mPtUYl0p4)

# Timing Table
|                 | Single | Multithreaded |    GPU    |
|-----------------|--------|---------------|-----------|
|average SPF |  2.182  |     1.523      |   1.278   |

Since the standard unit of video (frames per second) is not applicable when dealing with the incredibly under-powered Pi 3, seconds per frame (SPF) was used instead. Time was measured from the import of the Mat object to the display of the filtered image. For the multithreaded program, the average time of all threads was measured and divided by the number of buffered frames. 

## Optimizations
This program is an even further improvement upon the last assignment(s), as it impliments GPGPU encoding for the Sobel and Grayscale kernerls. 

The Pi 3 has a [VideoCore IV Graphics coproccessor](https://en.wikipedia.org/wiki/VideoCore#Table_of_SoCs_adopting_VideoCore_SIP_blocks) with roughly 24GFLOPS of compute power, not excedingly fast, but enough to see marginal performance gains. The Grayscale and Sobel shader cores are created in OpenCL, support for which is integrated into OpenCV (through the TransparentAPI OpenCV-OpenCL integration utilities), and the [VC4C compiler](https://github.com/doe300/VC4C) providing partial OpenCL 1.2 support on the RPi VideoCore IV. 

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