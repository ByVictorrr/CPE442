#include <iostream>
#include <inttypes.h> /* for PRIu64 definition */
#include <pthread.h>
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "threadhandler.h"
#include <arm_neon.h>

using namespace cv;
using namespace std;

#define FRAMEBUFSIZE 3 /* On Pi 4, buffer 3 frames, 4th is reserved for UI thread */
#define SOBEL_ROWS 3
#define SOBEL_COLS 3

struct sobel_weight{
    int rows=SOBEL_ROWS;
    int cols=SOBEL_COLS;
    int8_t w_x[SOBEL_ROWS][SOBEL_COLS] = {
                   {1, 0, -1},
                   {2, 0, -2},
                   {1, 0, -1}
               };

    int8_t w_y[SOBEL_ROWS][SOBEL_COLS] = {
                   {1, 2, 1},
                   {0, 0, 0},
                   {-1,-2,-1}
               };
};

void gray_scale(const uint8_t *gray_weights, Mat *img, Mat *gray)
{
    CV_Assert(img->type() == CV_8UC3);
    int rows = img->rows;
    int cols = img->cols;

    if(!img->isContinuous() || !gray->isContinuous()){
        return;
    }
    uint8x8_t scales = vld1_u8(gray_weights);
    uint8_t *_img = img->ptr<uchar>(0);
    uint8_t *_gray = gray->ptr<uchar>(0);
    uint16x8_t temp;
    uint8x8_t rgb;
    long long group_pixels = (rows*cols);
    for (long long pixel = 0; pixel < (group_pixels&~0x2); pixel+=2)
    {
        rgb = vld1_u8(&_img[pixel*3]);
        temp = vmull_u8(rgb, scales);
        _gray[pixel] = (vgetq_lane_u16(temp, 0) + vgetq_lane_u16(temp, 1) + vgetq_lane_u16(temp, 2))/100 ;
        _gray[pixel+1] = (vgetq_lane_u16(temp, 3) + vgetq_lane_u16(temp, 4) + vgetq_lane_u16(temp, 5))/100 ;

    }

    // cleanup
    for (long long pixel = (group_pixels&~0x2); pixel < group_pixels; pixel++){
        rgb = vld1_u8(&_img[pixel*3]);
        temp = vmull_u8(rgb, scales);
        _gray[pixel] = (vgetq_lane_u16(temp, 0) + vgetq_lane_u16(temp, 1) + vgetq_lane_u16(temp, 2))/100 ;
    }

}

uchar window_product(Mat *gray, const struct sobel_weight *W)
{
    int productX = 0, productY=0, product=0;
    int8x16_t results_x, results_y, gray_v, x, y;

    const uchar *gray_row = gray->ptr<uchar>(0);
    const int8_t * wx_row = W->w_x[0];
    const int8_t * wy_row = W->w_y[0];
    // Step 1 - Load the parms 
    x=vld1q_s8((const int8_t *)wx_row);
    y=vld1q_s8((const int8_t *)wy_row);
    gray_v=vld1q_s8((const int8_t *)gray_row);

    // Step 2 - vector multiply 
    results_x = vmulq_s8(x, gray_v);
    results_y = vmulq_s8(y, gray_v);
    productX = vgetq_lane_s8(results_x, 0) + vgetq_lane_s8(results_x, 1) 
              +vgetq_lane_s8(results_x, 2) + vgetq_lane_s8(results_x, 3)
              +vgetq_lane_s8(results_x, 4) + vgetq_lane_s8(results_x, 5) 
              +vgetq_lane_s8(results_x, 6) + vgetq_lane_s8(results_x, 7)
              +vgetq_lane_s8(results_x, 8) ;

    productY = vgetq_lane_s8(results_y, 0) + vgetq_lane_s8(results_y, 1) 
              +vgetq_lane_s8(results_y, 2) + vgetq_lane_s8(results_y, 3)
              +vgetq_lane_s8(results_y, 4) + vgetq_lane_s8(results_y, 5) 
              +vgetq_lane_s8(results_y, 6) + vgetq_lane_s8(results_y, 7)
              +vgetq_lane_s8(results_y, 8) ;

    product+=productX+productY;

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
Mat getWindow(Mat &img, int startCol, int startRow, int cols, int rows)
{
    Rect window(startCol, startRow, cols, rows);
    Mat grayWin = img(window);
    return grayWin;
}
void sobel_filter(const struct sobel_weight *W, Mat *gray, Mat *sobel){
    int rows = gray->rows;
    int W_rows = W->rows;
    int W_cols = W->cols;
    int cols = gray->cols;

    for(int row=0; row < rows; row++)
        for(int col=0; col < cols; col++){
            // For each pixel
            int startRow = row - (int)(W_rows/2);
            int startCol = col - (int)(W_cols/2);
            if(startRow < 0 || startCol < 0 || startRow+W_rows-1 >= rows  || startCol+W_cols-1 >= cols)
                continue;

            Mat &&grayWin = getWindow(*gray, startCol, startRow, SOBEL_COLS, SOBEL_ROWS);
            sobel->at<uchar>(row,col) = window_product(&grayWin, W);
        }
}
void *t_cvt_sobel(void *data)
{
    Mat *img = (Mat *)data;
    Mat *gray = new Mat(img->size(), CV_8UC1);
    Mat *sobel = new Mat(img->size(), CV_8UC1);
    const struct sobel_weight SOBEL_WEIGHTS;
    const uint8_t GRAY_WEIGHTS[8] = {
                                    (uint8_t)(.0722*100), (uint8_t)(.7152*100), (uint8_t)(.2126*100), 
                                    (uint8_t)(.0722*100), (uint8_t)(.7152*100), (uint8_t)(.2126*100)
                                    };
    // Step 1 - get gray mat
    gray_scale(GRAY_WEIGHTS, img, gray);
    // Step 2 - apply sobel filter
    sobel_filter(&SOBEL_WEIGHTS, gray, sobel);

    free(gray);
    free(data);
    return (void *)sobel;
}
int main(int argc, char *argv[])
{
    VideoCapture inputVideo;

    ThreadHandler workers(FRAMEBUFSIZE);
    double displayTime = 33.36; // in miliseconds (33.36 = 29.97fps, std 30hz video)
    Mat *frame;
    size_t frameNum = 0, qdframes = 0;
    bool isEmpty = false;
   
    //parse args
    if (argc < 2)
    {
        cerr << "sobel <video_file> " << endl;
        exit(EXIT_FAILURE);
    }
    // use -i cmdline flag for single frame
    if (argc == 3)
    {
        if (argv[2][0] == '-' && argv[2][1] == 'i')
        {
            displayTime = 0;
        }
    }

    //open video for decode
    if (!inputVideo.open(argv[1]))
    {
        cerr << "Not able to open " << argv[1] << endl;
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        // add FRAMEBUFSIZE frames to buffer queue
        while (qdframes < FRAMEBUFSIZE)
        {
            Mat *img = new Mat;
            inputVideo.read(*img);
            if (img->empty())
            {
                isEmpty = true;
                break;
            }

            //make new process thread
            //with id for circular buffer
            if (workers.create(frameNum + qdframes, (void *)img, (t_func)t_cvt_sobel) < 0)
            {
                perror("thread in use");
            }
            qdframes++;
        }

        //join based on ID
        if (workers.join(frameNum, (void **)&frame) < 0)
        {
            exit(EXIT_FAILURE);
        }

        //once data is available, show
        imshow("sobel filter", *frame);
        waitKey(displayTime); // display frame for N ms 
        qdframes--;
        frameNum++;

        free(frame);

        //check if done
        if (isEmpty && (qdframes == 0))
            break;
    }

    return 0;
}