
#include <iostream>
//#include "opencv2/core/types.hpp"

#include <opencv2/highgui.hpp>

float ITU_R(uint8_t R, uint8_t G, uint8_t B){
    return .2126*R+.7152*G+.0722*B;
}

void gray_scal(float (*gray_alg)(uint8_t, uint8_t, uint8_t), cv::Mat & img){
    for(int row=0; row < img.rows; row++)
        for(int col=0; col < img.cols; col++){
            // Step 1 - get pixel
            cv::Vec3b & color = img.at<cv::Vec3b>(col, row);
            // Step 2 - change the color
            img.at<cv::Vec3b>(cv::Point(col, row)) = gray_alg(color[0], color[1]make -j$(nproc)
        exit(EXIT_FAILURE);
    }

    if(!vc.open(argv[1])){
        std::cerr << "Not able to open " << argv[1] << std::endl;
        exit(EXIT_FAILURE);
    }


    while(1){
        vc >> img;
        if(img.empty())
            break;

       gray_scal(ITU_R, img);
       cv::imshow("w", img);
       cv::waitKey(20); // wait 20s to display frame
    }


    return 0;    
}