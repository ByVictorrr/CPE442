
#include <iostream>
//#include "opencv2/core/types.hpp"

#include "opencv2/highgui.hpp"
using namespace cv;
using namespace std;

enum VEC3B_COLORS {BLUE, GREEN, RED};
struct gradient{
    int **x;
    int **y;
    int rows;
    int cols;
};

Vec3b ITU_R(uchar R, uchar G, uchar B){
    return Vec3b((uchar)(.0722*B),(uchar)(.7152*G),(uchar)().2126*R));
}

Mat gray_scal(Vec3b (*gray_alg)(uint8_t, uint8_t, uint8_t), const Mat &img)
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
    if(G.cols < 3  && G.rows < 3)
        return nullptr_t;

    int startX = abs(col-(G.x-2));
    int startY = abs(row-(G.y-2));
    Mat small; 
    img(Rect(startX, startY, G.cols, G.rows)).copyTo(small);
    return small;
}
int window_product(Mat &winImg, const struct gradient *G){
    int productX=0;
    int productY=0;
    for(int row=0; row < winImg.rows; row++){
        for(int col=0; col < winImg.cols; col++){
            productX+=G.x[row][col]*winImg.at(row, col);
            productY+=G.y[row][col]*winImg.at(row, col);
        }
    }
    return productX+productY;
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
                Mat &window = get_windowSize(G, col, row, grayImg);
                if(window == nullptr_t)
                    return nullptr_t
                // Step 2 - get the |G| = productX+productY
                sobel.at(row, col) = window_product(window, G);
                
            }
         }

    return sobel;
}
int main(int argc, char *argv[]){
    VideoCapture vc;
    Mat img, gray;
    struct gradient G;
    int G_x[3][3] = {{-1, 0, 1},{-2,0,2},{-1,0,1}};
    int G_y[3][3] = {{-1, 0, 1},{-2,0,2},{-1,0,1}};

    G.x = G_x;
    G.y = G_y;
    G.rows = 3;
    G.cols = 3;
    

    if(argc !=2 ){
        cerr << "sobel <vide_file> " << endl; 
        exit(EXIT_FAILURE);
    }

    if(!vc.open(argv[1])){
        cerr << "Not able to open " << argv[1] << endl;
        exit(EXIT_FAILURE);
    }


    while(1){
        vc >> img;
        if(img.empty())
            break;

       // Step 1 - get gray mat
       gray = gray_scal(ITU_R, img);
       // Step 2 - get sober filter
       sobel_filter(&G, gray);

       imshow("w", gray);
       waitKey(20); // wait 20s to display frame
    }


    return 0;    
}
