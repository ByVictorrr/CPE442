
#include <iostream>
//#include "opencv2/core/types.hpp"

#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

enum VEC3B_COLORS {BLUE, GREEN, RED};
struct gradient{
    vector <vector<int>> x;
    vector <vector<int>> y;
    int rows;
    int cols;
};

Vec3b ITU_R(uchar R, uchar G, uchar B){
    return Vec3b((uchar)(.0722*B),(uchar)(.7152*G),(uchar)(.2126*R));
}

Mat gray_scal(Vec3b (*gray_alg)(uint8_t, uint8_t, uint8_t), Mat &img)
{
    Mat gray(img.rows, img.cols, img.type());
    for(int row=0; row < img.rows; row++)
        for(int col=0; col < img.cols; col++){
            // Step 1 - get pixel
            Vec3b & color = img.at<Vec3b>(col, row);
            // Step 2 - change the color
            gray.at<Vec3b>(row,col) = gray_alg(color[RED], color[GREEN], color[BLUE]);
    }
    return gray;
}
// Needs at least 3x3 window size
Mat get_windowSize(const struct gradient *G, int col, int row,Mat &img){
    if(G->cols < 3  && G->rows < 3)
        exit(EXIT_FAILURE);

    int startX = abs(col-(G->cols-2));
    int startY = abs(row-(G->rows-2));
    Mat small; 
    img(Rect(startX, startY, G->cols, G->rows)).copyTo(small);
    return small;
}
int window_product(Mat &winImg, const struct gradient *G){
    int productX=0;
    int productY=0;
    for(int row=0; row < winImg.rows; row++){
        for(int col=0; col < winImg.cols; col++){
            productX+=G->x[row][col]*winImg.at<uchar>(row, col);
            productY+=G->y[row][col]*winImg.at<uchar>(row, col);
        }
    }
    return sqrt(productX*productX+productY*productY);
}

Mat sobel_filter(const struct gradient *G, Mat &grayImg){
    int rows = grayImg.rows;
    int cols = grayImg.cols;
    Mat sobel(rows, cols, grayImg.type());
    for(int row=0; row < rows; row++)
        for(int col=0; col < cols; col++){
            // if on first col or first row dont use the filter
            // or if row is on the last
            if((row == 0 || col == 0) || (row == rows-1 || col == cols-1) ){
                continue;
            }else{
                // Step 1 - get pixels
                Mat window = get_windowSize(G, col, row, grayImg);
                // Step 2 - get the |G| = productX+productY
                sobel.at<uchar>(row, col) = window_product(window, G);
                
            }
         }

    return sobel;
}
int main(int argc, char *argv[]){
    VideoCapture vc;
    Mat img, gray, sobel;
    struct gradient G;

    G.x = {{-1, 0, 1},{-2,0,2},{-1,0,1}};
    G.y = {{1, 2, 1},{0,0,0},{-1,-2,-1}};
    G.rows = G.x.size();
    G.cols = G.x[0].size();

    

    if(argc !=2 ){
        cerr << "sobel <vide_file> " << endl; 
        exit(EXIT_FAILURE);
    }

    //img = imread(argv[1], IMREAD_COLOR);

    if(!vc.open(argv[1])){
        cerr << "Not able to open " << argv[1] << endl;
        exit(EXIT_FAILURE);
    }


    while(1){
        vc >> img;
        if(img.empty())
            break;

       // Step 1 - get gray mat
       //gray = gray_scal(ITU_R, img);
        cvtColor(img, gray, COLOR_BGR2GRAY);

       // Step 2 - get sober filter
       sobel = sobel_filter(&G, gray);

       imshow("w", sobel);
       waitKey(20); // wait 20s to display frame
    }


    return 0;    
}
