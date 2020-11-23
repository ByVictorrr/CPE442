int constant W_x[3][3] = {
	{1, 0, -1},
	{2, 0, -2},
	{1, 0, -1}
};
int constant W_y[3][3] = {
	{1, 2, 1},
	{0, 0, 0},
	{-1, -2, 1}
};
__kernel void
window_product(__read_only image2d_t input, __write_only image2d_t output, __global int rows, __global int cols){
	int x = get_global_id(0);
	int y = get_global_id(1);
	int4 result = 0;
	// Take care of the edge pixels
	if (x == 0 | y==0 | y==rows-1 | x==cols-1){
		uint4 p = readImage(input, (int2)(x,y));
		write_image(output, (int2)(x,y), p);
	}else{
		// l-left, lu-left up, ld-left down

		int4 _mid = read_imageui(input, (int2)(x,y));
		int _dwn = read_imageui(input, (int2)(x,y+1));
		int _up = read_imageui(input, (int2)(x,y-1));

		int4 _l = read_imageui(input, (int2)(x-1,y));
		int4 _r = read_imageui(input, (int2)(x+1,y));

		int4 _lu = read_imageui(input, (int2)(x-1,y-1));
		int4 _ld = read_imageui(input, (int2)(x-1,y+1));
		int4 _ru = read_imageui(input, (int2)(x+1,y-1));
		int4 _rd = read_imageui(input, (int2)(x+1,y+1));

		result[0] = _l[0] * W_x[1][0] + _lu[0]*W_x[0,0] + _up[0]*W_x[0][1] + _ru[0]*W_x[0][2] + _r[0]*W_x[1][2]
				+ _rd[0]*W_x[2][2] +_dwn[0]*W_x[2][1] + _ld[0]*W_x[2][0] + _mid[0] * W_x[1][1];
				
		result[0] += _l[0] * W_y[1][0] + _lu[0]*W_y[0,0] + _up[0]*W_y[0][1] + _ru[0]*W_y[0][2] + _r[0]*W_y[1][2]
				+ _rd[0]*W_y[2][2] +_dwn[0]*W_y[2][1] + _ld[0]*W_y[2][0] + _mid[0] * W_y[1][1];

		if(result[0] > 255){
			result[0] = 255;
		}else if (result[0] < 0){
			result[0] = 0;
		}
		write_image(output, (int2)(x,y), result);
	}
}
