#include "opencl-c.h"
#include "openv.h"

/*
kernel void windowProduct(__global __image2d_t a,
                            __global __image2d_t b,
                           __global uchar *res)
{
    a[i]
}
*/

int main(){

cl_int ret_code = CL_SUCCESS;
cl_mem shore_mask_buff = clCreateBuffer(ocl_context, CL_MEM_COPY_HOST_PTR,                                         
                                        shore_mask.cols * shore_mask.rows * sizeof(uint8_t), (void*)shore_mask.data, &ret_code);

}