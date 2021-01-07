#include<iostream>

#include <opencv2/opencv.hpp>

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;
using namespace std;

const double sigma = 10.0;
const int ksize = 3;


int main( int argc, char** argv ) {
    Mat src;
    // the first command-line parameter must be a filename of the source image
    if( argc != 2 || !(src=imread(argv[1])).data)
        return -1;

    Mat dst = Mat::zeros(src.rows, src.cols, CV_8UC3);

    imshow( "Original", src );

    Mat grayImage(src.rows, src.cols, CV_8UC1);
    cvtColor(src, grayImage, COLOR_BGR2GRAY);
    // GaussianBlur(grayImage, grayImage, cv::Size(ksize, ksize), sigma);

    Mat threshImg1, threshImg2;
    cv::threshold(grayImage, threshImg1, 75, 255, 0);

    imshow( "Umbralizada", threshImg1);


    cv::Mat e2 = getStructuringElement(MORPH_RECT, Size(3,3));
    
    Mat src4;

    cv::erode(threshImg1, threshImg1, e2);
    cv::erode(threshImg1, src4, e2);

    Mat src5;
    bitwise_xor(threshImg1, src4, src5);

    imshow( "Contorno", src5);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours( src5, contours, hierarchy,
        RETR_TREE, CHAIN_APPROX_SIMPLE );

    // iterate through all the top-level contours,
    // draw each connected component with its own random color

    for(int idx = 0; idx >= 0; idx = hierarchy[idx][0] ) {
        Scalar color( rand()&255, rand()&255, rand()&255 );
        drawContours( dst, contours, idx, color, 1, 8, hierarchy, 0);
    }

    imshow( "Contornos", dst );

    waitKey(0);
}