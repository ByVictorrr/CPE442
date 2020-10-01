
#include <iostream>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"

#include "threadhandler.h"

using namespace cv;
using namespace std;

enum VEC3B_COLORS
{
    BLUE,
    GREEN,
    RED
};

#define FRAMEBUFSIZE 3 /* On Pi 4, buffer 3 frames, 4th is reserved for UI thread */
#define SOBEL_ROWS 3
#define SOBEL_COLS 3
struct sobel_weight
{
    const int x[SOBEL_ROWS][SOBEL_COLS] = {
        {1, 0, -1},
        {2, 0, -2},
        {1, 0, -1}};
    const int y[SOBEL_ROWS][SOBEL_COLS] = {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}};
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
            gray->at<uchar>(row, col) = gray_alg(channels[RED], channels[GREEN], channels[BLUE]);
        }
    }
}

uchar window_product(Mat *gray, const struct sobel_weight *W, int startX, int startY)
{
    int product = 0;
    for (int row = startY; row < startY + SOBEL_ROWS; row++)
    {
        for (int col = startX; col < startX + SOBEL_COLS; col++)
        {
            product += W->x[row - startY][col - startX] * gray->at<uchar>(col, row);
            product += W->y[row - startY][col - startX] * gray->at<uchar>(col, row);
        }
    }
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

void sobel_filter(const struct sobel_weight *W, Mat *gray, Mat *sobel)
{
    int rows = gray->rows;
    int cols = gray->cols;

    for (int row = 1; row < rows - 1; row++)
        for (int col = 1; col < cols - 1; col++)
            sobel->at<uchar>(row, col) = window_product(gray, W, row - 1, col - 1);
}

void *t_cvt_sobel(void *data)
{
    Mat *img = (Mat *)data;
    Mat *gray = new Mat(img->size(), CV_8UC1);
    Mat *sobel = new Mat(img->size(), CV_8UC1);
    sobel_weight sw;

    // Step 1 - get gray mat
    gray_scale(ITU_R, img, gray);
    // Step 2 - apply sobel filter
    sobel_filter(&sw, gray, sobel);

    free(gray);
    free(data);
    return (void *)sobel;
}

int main(int argc, char *argv[])
{
    VideoCapture inputVideo;
    double displayTime = 33.36;  // display frame for 33.36ms (29.97fps, std video)

    //parse args
    if (argc < 2)
    {
        cerr << "sobel <video_file> " << endl;
        exit(EXIT_FAILURE);
    }
    if (argc == 3)
    {
        if (argv[2][0] == '-')
        {
            if (argv[2][1] == 'i')
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
        if (workers.join(frameNum, (void **)&frame, sizeof(Mat)) < 0)
        {
            exit(EXIT_FAILURE);
        }

        //once data is available, show
        imshow("sobel filter", *frame);
        waitKey(displayTime);
        qdframes--;
        frameNum++;

        free(frame);

        //check if done
        if (isEmpty && (qdframes == 0))
            break;
    }

    return 0;
}
