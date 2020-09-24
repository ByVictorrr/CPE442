#include <opencv2/core.hpp>

const char Gx = [[-1, 0, 1], [-2, 0, 2], [-1, 0, 1]];
const char Gy = [[-1, -2, -1], [0, 0, 0], [1, 2, 1]];

using namespace std;
using namespace cv;

void bw_filter(Mat &, Mat &);
void sobel_filter(Mat &, Mat &);

int main(int argc, char *argv[])
{
   VideoCapture cap("chaplin.mp4");

   if (!cap.isOpened())
   {
      cout << "Error opening video stream or file" << endl;
      return -1;
   }

   while (1)
   {

      Mat frame;
      // Capture frame-by-frame
      cap >> frame;

      // If the frame is empty, break immediately
      if (frame.empty())
      {
         break;
      }
      Mat gframe, sframe;

      // process frame
      bw_filter(frame, gframe);

      sobel_filter(gframe, sframe);

      // Display the resulting frame
      imshow("Frame", sframe);

      // Press  ESC on keyboard to exit
      char c = (char)waitKey(25);
      if (c == 27)
         break;
   }

   cap.release();

   // Closes all the frames
   destroyAllWindows();

   return 0;
}

void bw_filter(Mat &frame, Mat &bwframe)
{
   int rows = frame.rows, cols = frame.cols;

   bwframe.create(frame.size(), CV_8UFC(1)); //CV_32FC(nbChannels))

   if (frame.isContinuous() && bwframe.isContinuous())
   {
      cols = rows * cols;
      rows = 1;
   }

   for (int row = 0; row < rows; row++)
   {
      const uchar *src_ptr = frame.ptr<uchar>(row);
      uchar *dst_ptr = bwframe.ptr<uchar>(row);

      for (int col = 0; col < cols; col++)
      {
         dst_ptr[col] = (uchar)(src_ptr[0] * 0.2126f + src_ptr[1] * 0.7152f + src_ptr[2] * 0.0722f);
         src_ptr += 3;
      }
   }
}

void sobel_filter(Mat &frame, Mat &sframe)
{
   int rows = frame.rows, cols = frame.cols;

   bwframe.create(frame.size(), CV_8UFC(1)); //CV_32FC(nbChannels))

   if (frame.isContinuous() && bwframe.isContinuous())
   {
      cols = rows * cols;
      rows = 1;
   }

   for (int row = 1; row < rows - 1; row++)
   {
      const uchar *src_ptr = frame.ptr<uchar>(row);
      uchar *dst_ptr = bwframe.ptr<uchar>(row);

      for (int col = 1; col < cols - 1; col++)
      {

         S1 = sum(sum(Gx.*A(i:i+2,j:j+2)))
			S2 = sum(sum(Gy.*A(i:i+2,j:j+2)))

			mag(i+1, j+1) = sqrt(S1.^2+S2.^2)


         src_ptr += 1; //bw only had 1 channel?
      }
   }
}