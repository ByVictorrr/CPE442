__kernel void Gray(__global unsigned char* input, __global unsigned char* output)
{
    int gid = get_global_id(0);
    // BGR
    int B = 0;
    int G = 1;
    int R = 2;
    output[gid] =  0.0722 * input[3*gid+B] + 0.7152 * input[3*gid+G] + 0.2126 * input[3*gid+R];
}

__kernel void Sobel(__global unsigned char* gray, __global unsigned char* sobel, unsigned int size, unsigned int width, unsigned int height)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    if ((x > 0) && (x < (width-1)) && (y > 0 && (y < (height-1))))
    {
        int x_product = (1 * gray[width * (y-1) + (x-1)]) + (0 * gray[width * (y-1) + (x)]) + (-1 * gray[width * (y-1) + (x+1)]) +
                        (2 * gray[width * (y  ) + (x-1)]) + (0 * gray[width * (y  ) + (x)]) + (-2 * gray[width * (y  ) + (x+1)]) +
                        (1 * gray[width * (y+1) + (x-1)]) + (0 * gray[width * (y+1) + (x)]) + (-1 * gray[width * (y+1) + (x+1)]);

        int y_product = ( 1 * gray[width * (y-1) + (x-1)]) + ( 2 * gray[width * (y-1) + (x)]) + ( 1 * gray[width * (y-1) + (x+1)]) +
                        ( 0 * gray[width * (y  ) + (x-1)]) + ( 0 * gray[width * (y  ) + (x)]) + ( 0 * gray[width * (y  ) + (x+1)]) +
                        (-1 * gray[width * (y+1) + (x-1)]) + (-2 * gray[width * (y+1) + (x)]) + (-1 * gray[width * (y+1) + (x+1)]);
        
        int product = x_product + y_product;
        if (product > 255) {
            product = 255;
        }
        if (product < 0) {
            product = 0;
        }
        sobel[width * y + x] = product;
    }
}