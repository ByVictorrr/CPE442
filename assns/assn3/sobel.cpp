
#include <iostream>
#include <pthread.h>
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

enum VEC3B_COLORS {BLUE, GREEN, RED};
#define NUM_THREADS 4

struct sobel_weight{
    vector<vector<int>> w_x;
    vector<vector<int>> w_y;
};
struct thread_sob_fil_parm{
    uchar (*gray_alg)(uchar, uchar, uchar);
    const struct sobel_weight *W;
    Mat *img;
    Mat *sobel;
};

uchar ITU_R(uchar R, uchar G, uchar B);
void gray_scal(uchar (*gray_alg)(uchar,uchar,uchar),Mat &img, Mat &gray);
uchar window_product(Mat &gray, const struct sobel_weight *W, int startX, int startY);

void *sobel_filter(void *sob_fil){
    Mat gray;
    struct thread_sob_fil_parm *parm = (struct thread_sob_fil_parm*)sob_fil;
    // Step 1 - turn the image into gray 
    gray_scal(parm->gray_alg, *(parm->img), gray);
    parm->sobel->create(gray.size(), gray.type());
    // Step 2 - get the rows/cols of gray frame and window rows/cols
    int rows = gray.rows;
    int W_rows =  parm->W->w_x.size();
    int W_cols =  parm->W->w_x[0].size();
    int cols = gray.cols;

    for(int row=0; row < rows; row++)
        for(int col=0; col < cols; col++){
            // For each pixel
            int startRow = row - (int)(W_rows/2);
            int startCol = col - (int)(W_cols/2);
            // Check if the window taken from gray is on the Mat
            if(startRow < 0 || startCol < 0 || startRow+W_rows-1 > rows  || startCol+W_cols-1 > cols)
                continue;
            parm->sobel->at<uchar>(row,col) = window_product(gray, parm->W, startRow, startCol);
        }
        pthread_exit(0);
}

int main(int argc, char *argv[]){
    VideoCapture inputVideo;
    Mat sobel;
    struct thread_sob_fil_parm parms[NUM_THREADS];
    pthread_t tid[NUM_THREADS];
    Mat imgs[NUM_THREADS];
   
    int runningThreads = 4;

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

    
    while(runningThreads == NUM_THREADS){

        // Step 1 - go through threads and assign each one
        for(int i = 0; i < NUM_THREADS; i++){
            parms[i].gray_alg = ITU_R;
            parms[i].W = &WEIGHTS;
            parms[i].sobel = new Mat;
            inputVideo >> imgs[i];
            parms[i].img = &imgs[i];
            runningThreads = i+1;
            if(parms[i].img->empty())
                break;
            
            pthread_create(&tid[i], NULL, sobel_filter, &parms[i]);
        }
        for(int i = 0; i < NUM_THREADS; i++){
            pthread_join(tid[i], NULL);
        }
        for(int i = 0; i < runningThreads; i++){
            if(!parms[i].sobel->empty()){
                imshow("sobel filter", *(parms[i].sobel));
                waitKey(1); // wait 20s to display frame
                delete parms[i].sobel;
            }
        }

    }


    return 0;    
}


uchar ITU_R(uchar R, uchar G, uchar B){
    return .0722*B+.7152*G+.2126*R;
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

