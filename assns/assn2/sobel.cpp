
#include <iostream>
//#include "opencv2/core/types.hpp"

#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

enum VEC3B_COLORS {BLUE, GREEN, RED};

struct sobel_weight{
    vector<vector<int>> w_x;
    vector<vector<int>> w_y;
};

uchar ITU_R(uchar R, uchar G, uchar B){
    return .0722*B+.7152*G+.2126*R;
}

void gray_scal(uchar (*gray_alg)(uchar,uchar,uchar),Mat &img, Mat &gray)
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

uchar window_product(Mat &gray, const struct sobel_weight *W, int startX, int startY){
    int W_rows =  W->w_x.size();
    int W_cols =  W->w_x[0].size();
    int product = 0;
    for(int row=startY; row < startY+W_rows; row++){
        for(int col=startX; col < startX+W_cols; col++){
            product += W->w_x[row-startY][col-startX]*gray.at<uchar>(col, row);
            product += W->w_y[row-startY][col-startX]*gray.at<uchar>(col, row);
        }
    }
    if(product > 255){
        return 255;
    }else if(product < 0){
        return 0;
    }
    return product; 
}
void sobel_filter(const struct sobel_weight *W, Mat &gray, Mat & sobel){
    int rows = gray.rows;
    int cols = gray.cols;
    int W_rows =  W->w_x.size();
    int W_cols =  W->w_x[0].size();
    sobel.create(gray.size(), gray.type());

    for(int row=0; row < rows; row++)
        for(int col=0; col < cols; col++){
            // For each pixel
            int startRow = row - (int)(W_rows/2);
            int startCol = col - (int)(W_cols/2);
            // Check if the window taken from gray is on the Mat
            if(startRow < 0 || startCol < 0 || startRow+W_rows-1 > rows  || startCol+W_cols-1 > cols)
                continue;
            sobel.at<uchar>(row,col) = window_product(gray, W, startRow, startCol);
        }
}

int main(int argc, char *argv[]){
    VideoCapture inputVideo;
    Mat img, gray, sobel;

    const struct sobel_weight WEIGHTS = {
               {
                   {1, 0, -1},
                   {2, 0, -2},
                   {1, 0, -1}
               },
               {
                   {1, 2, 1},
                   {0, 0, 0},
                   {-1,-2,-1}
               }
    };



    if(argc !=2 ){
        cerr << "sobel <vide_file> " << endl; 
        exit(EXIT_FAILURE);
    }

    if(!inputVideo.open(argv[1])){
        cerr << "Not able to open " << argv[1] << endl;
        exit(EXIT_FAILURE);
    }

   
    while(1){
        inputVideo >> img;
        if(img.empty())
            break;

       // Step 1 - get gray mat
       gray_scal(ITU_R, img, gray);
       // Step 2 - get sober filter
       sobel_filter(&WEIGHTS, gray, sobel);
       imshow("sobel filter", sobel);
       waitKey(20); // wait 20s to display frame
    }


    return 0;    
}
