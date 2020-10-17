#include <iostream>
#include <pthread.h>
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "threadhandler.h"
#include <arm_neon.h>
using namespace cv;
using namespace std;

enum VEC3B_COLORS {BLUE, GREEN, RED};
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
uchar ITU_R(uchar R, uchar G, uchar B)
{
    return .0722 * B + .7152 * G + .2126 * R;
}

void gray_scale(uchar (*gray_alg)(uchar, uchar, uchar), Mat *img, Mat *gray)
{
    int rows = img->rows;
    int cols = img->cols;

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            Vec3b &channels = img->at<Vec3b>(row, col);
            //img->ptr<Vec3b>(row, col)->mul({.0772, .7152, .2126});

            gray->at<uchar>(row, col) = gray_alg(channels[RED], channels[GREEN], channels[BLUE]);
        }
    }
}

uchar window_product(Mat *gray, const struct sobel_weight *W)
{
    int productX = 0, productY=0, product=0;
    int W_rows =  W->rows;
    int W_cols =  W->cols;
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
    for(int i=0; i < W_rows*W_cols; i++){
        productX+=vget_lane_s8(results_x, i);
        productY+=vget_lane_s8(results_y, i);
        /*
    productX= vgetq_lane_s8(results_x, 0) + vgetq_lane_s8(results_x, 1) + vgetq_lane_s8(results_x, 2) + vgetq_lane_s8(results_x, 3)
             +vgetq_lane_s8(results_x, 4) + vgetq_lane_s8(results_x, 5) + vgetq_lane_s8(results_x, 6) + vgetq_lane_s8(results_x, 7)
             +vgetq_lane_s8(results_x, 8) ;

    productY= vgetq_lane_s8(results_y, 0) + vgetq_lane_s8(results_y, 1) + vgetq_lane_s8(results_y, 2) + vgetq_lane_s8(results_y, 3)
             +vgetq_lane_s8(results_y, 4) + vgetq_lane_s8(results_y, 5) + vgetq_lane_s8(results_y, 6) + vgetq_lane_s8(results_y, 7)
             +vgetq_lane_s8(results_y, 8) ;
             */
    }

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
    const struct sobel_weight WEIGHTS;


    // Step 1 - get gray mat
    gray_scale(ITU_R, img, gray);
    // Step 2 - apply sobel filter
    sobel_filter(&WEIGHTS, gray, sobel);

    free(gray);
    free(data);
    return (void *)sobel;
}
int main(int argc, char *argv[])
{
    VideoCapture inputVideo;
    double displayTime = 33.36; // in miliseconds (33.36 = 29.97fps, std 30hz video)

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

    ThreadHandler workers(FRAMEBUFSIZE);

    Mat *frame;
    size_t frameNum = 0, qdframes = 0;
    bool isEmpty = false;
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