## CPE 442 : Final Project : GPU Accelerated Sobel Filter

## Author(s)
* Victor Delaplaine
* Tristan Chutka

## Video link
* [Video demonstrating the program with GPU acceleration.](https://youtu.be/g0mPtUYl0p4)

## Timing Table
|           | Single | Multithreaded |    GPU    |
|-----------|--------|---------------|-----------|
|average fps|   10   |     29.97     |   29.97   |


### Optimizations
This program is an even further improvement upon the last assignment(s), as it impliments GPGPU encoding for the Sobel and Grayscale kernerls. 

The Pi 3 has a [VideoCore IV Graphics coproccessor](https://en.wikipedia.org/wiki/VideoCore#Table_of_SoCs_adopting_VideoCore_SIP_blocks) with roughly 24GFLOPS of compute power, not excedingly fast, but enough to see marginal performance gains. The Grayscale and Sobel shader cores are created in OpenCL, support for which is integrated into OpenCV (through the TransparentAPI OpenCV-OpenCL integration utilities), and the [VC4C compiler](https://github.com/doe300/VC4C) providing partial OpenCL support on the RPi VideoCore IV. 

