__kernel void gray_scale(__global uchar *input, __global  uchar *output)
{
    int gid = get_global_id(0);
    // BGR
    int B = 0;
    int G = 1;
    int R = 2;
    output[gid] =  0.0722 * input[3*gid+B] + 0.7152 * input[3*gid+G] + 0.2126 * input[3*gid+R];
}

__kernel void sobel_product(__global uchar *gray, __global uchar *sobel, uchar size, uint width, uint height)
{
    int id = get_global_id(0);
    id *= size;
    //int max_index = (width * height) / size;
    int x = id % width;
    int y = id / width;

    if ((x == 0) || (y == 0) || (x == (width - 1)) || (y == (heigh - 1)))
    {
        sobel[id] = 0;
        return;
    }
    if ((x >= width) || (y >= height))
        return;

    // index = (width * y) + x;
    int x_product = (1 * gray[(width * (y-1) + (x-1)]) + (0 * gray[(width * (y-1) + (x)]) + (-1 * gray[(width * (y-1) + (x+1)]) +
                    (2 * gray[(width * (y  ) + (x-1)]) + (0 * gray[(width * (y  ) + (x)]) + (-2 * gray[(width * (y  ) + (x+1)]) +
                    (1 * gray[(width * (y+1) + (x-1)]) + (0 * gray[(width * (y+1) + (x)]) + (-1 * gray[(width * (y+1) + (x+1)]);

    int y_product = ( 1 * gray[(width * (y-1) + (x-1)]) + ( 2 * gray[(width * (y-1) + (x)]) + ( 1 * gray[(width * (y-1) + (x+1)]) +
                    ( 0 * gray[(width * (y  ) + (x-1)]) + ( 0 * gray[(width * (y  ) + (x)]) + ( 0 * gray[(width * (y  ) + (x+1)]) +
                    (-1 * gray[(width * (y+1) + (x-1)]) + (-2 * gray[(width * (y+1) + (x)]) + (-1 * gray[(width * (y+1) + (x+1)]);
                    
    sobel[id] = x_product + y_product;
}