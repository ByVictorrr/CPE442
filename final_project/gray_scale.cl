__kernel void gray_scale(__global uchar *input,
                         __global  uchar *output)
{
    int gid = get_global_id(0);
    // BGR
    int B = 0;
    int G = 1;
    int R = 2;
    output[gid] =  0.0722 * input[3*gid+B] + 0.7152 * input[3*gid+G] + 0.2126 * input[3*gid+R];
}
