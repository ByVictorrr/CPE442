__kernel void gray_scale(__global __image2d_t src, __global __image2d_t dst, __global uchar *res)
{
    int x = get_global_id(0);
    int y = get_global_id(1);
    int2 coord;
    ushort4 srcPixel; // RGBA (A = alpah)
    ushort destPixel;
    sampler_t samp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
    // Step 1 - set the coordinates
    coord[0] = x;
    coord[1] = y;

    // Step 2 - read the image pixel at that coord
    srcPixel = read_image(src, samp, coord);
    // Step 3 - gray scale that pixel
    destPixel = srcPixel[0]*.2126 + srcPixel[1] *.7152 + srcPixel[2] *.0722;
    // Step 4 - write_image to dest
    write_image()

}

__kernel void window_product(__global __image2d_t gray, uchar3 x[3], uchar3 y[3], uchar *ret)
{
    int productX = 0, productY=0, product=0;

    if (product > 255)
    {
        return 255;
    }
    else if (product < 0)
    {
        return 0;
    }
    return product;
}

