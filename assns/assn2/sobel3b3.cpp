
#include <iostream>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;
using namespace std;

enum VEC3B_COLORS
{
    BLUE,
    GREEN,
    RED
};

enum EVENT_FLAGS
{
    WAITING,
    READY,
    DONE
};
struct Frame
{
    uint8_t flags;
    Mat* frame;
};

#define SOBEL_ROWS 3
#define SOBEL_COLS 3
struct sobel_weight
{
    const int (*w_x)[SOBEL_ROWS];
    const int (*w_y)[SOBEL_ROWS];
};

uchar ITU_R(uchar R, uchar G, uchar B)
{
    return .0722 * B + .7152 * G + .2126 * R;
}

void gray_scal(uchar (*gray_alg)(uchar, uchar, uchar), Mat &img, Mat &gray)
{
    int rows = img.rows;
    int cols = img.cols;

    gray.create(img.size(), CV_8UC1);

    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            Vec3b &channels = img.at<Vec3b>(row, col);
            gray.at<uchar>(row, col) = gray_alg(channels[RED], channels[GREEN], channels[BLUE]);
        }
    }
}

uchar window_product(Mat &gray, const struct sobel_weight *W, int startX, int startY)
{
    int product = 0;
    for (int row = startY; row < startY + 3; row++)
    {
        for (int col = startX; col < startX + 3; col++)
        {
            product += W->w_x[row - startY][col - startX] * gray.at<uchar>(col, row);
            product += W->w_y[row - startY][col - startX] * gray.at<uchar>(col, row);
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

void sobel_filter(const struct sobel_weight *W, Mat &gray, Mat &sobel)
{
    int rows = gray.rows;
    int cols = gray.cols;
    sobel.create(gray.size(), gray.type());

    for (int row = 1; row < rows - 1; row++)
        for (int col = 1; col < cols - 1; col++)
            sobel.at<uchar>(row, col) = window_product(gray, W, row - 1, col - 1);
}

int main(int argc, char *argv[])
{
    uint frame;
    VideoCapture inputVideo;
    Mat img, gray, sobel;
    const int W_x[SOBEL_ROWS][SOBEL_COLS] = {
        {1, 0, -1},
        {2, 0, -2},
        {1, 0, -1}};
    const int W_y[SOBEL_ROWS][SOBEL_COLS] = {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}};
    const struct sobel_weight WEIGHTS = {W_x, W_y};

    if (argc != 2)
    {
        cerr << "sobel <vide_file> " << endl;
        exit(EXIT_FAILURE);
    }

    if (!inputVideo.open(argv[1]))
    {
        cerr << "Not able to open " << argv[1] << endl;
        exit(EXIT_FAILURE);
    }

    Frame framebuffer[3]; ///prerender 3 frames, 4th is UI thread
    //size - number of currenly avail frames

    while (1)
    {
        inputVideo >> img;
        if (img.empty()) {
            break;
        } else {
            //make new process thread
            //with id for circular buffer
        }

        //join
        //if data avail, add to framebuffer based on ID

        // Step 1 - get gray mat
        gray_scal(ITU_R, img, gray);
        // Step 2 - get sober filter
        sobel_filter(&WEIGHTS, gray, sobel);
        imshow("sobel filter", sobel);
        waitKey(20); // display frame for 20ms
    }

    return 0;
}
