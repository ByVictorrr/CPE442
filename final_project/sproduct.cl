__kernel void window_product(__global const uint8_t *sobel, __global uint8_t *gray, uchar size, uint width, uint height)
{
    int id = get_global_id(0);
    id *= size;
    int max_index = (width * height) / size;

    if (id >= max_index)
        return;

    int x = id % width;
    int y = id / width;

    // index = (width * y) + x;
    int x_product = (1 * gray[(width * (y-1) + (x-1)]) + (0 * gray[(width * (y-1) + (x)]) + (-1 * gray[(width * (y-1) + (x+1)]) +
                    (2 * gray[(width * (y  ) + (x-1)]) + (0 * gray[(width * (y  ) + (x)]) + (-2 * gray[(width * (y  ) + (x+1)]) +
                    (1 * gray[(width * (y+1) + (x-1)]) + (0 * gray[(width * (y+1) + (x)]) + (-1 * gray[(width * (y+1) + (x+1)]);

    int y_product = ( 1 * gray[(width * (y-1) + (x-1)]) + ( 2 * gray[(width * (y-1) + (x)]) + ( 1 * gray[(width * (y-1) + (x+1)]) +
                    ( 0 * gray[(width * (y  ) + (x-1)]) + ( 0 * gray[(width * (y  ) + (x)]) + ( 0 * gray[(width * (y  ) + (x+1)]) +
                    (-1 * gray[(width * (y+1) + (x-1)]) + (-2 * gray[(width * (y+1) + (x)]) + (-1 * gray[(width * (y+1) + (x+1)]);
                    
    sobel[id] = x_product + y_product;
}