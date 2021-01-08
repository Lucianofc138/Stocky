#include <iostream>
#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include "processingMisc/processingMisc.hpp"

#ifdef HAVE_OPENCV_XFEATURES2D
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
using namespace cv;
using namespace cv::xfeatures2d;
using std::cout;
using std::endl;


int main( int argc, char* argv[] )
{

    Mat img1 = imread( "samples/filas/prod_samp4/S4_P7.jpg", IMREAD_GRAYSCALE ); // features/S4_P9
    Mat img2 = imread( "samples/filas/sample4_1.jpg", IMREAD_GRAYSCALE );
    if ( img1.empty() || img2.empty() )
    {
        cout << "Could not open or find the image!\n" << endl;
        return -1;
    }
    imshow("1", img1 );
    imshow("2", img2 );

    std::vector<cv::Rect> boxes;
    stky::scanFeaturesSlidingWindow(img1, img2, boxes);

    return 0;
}

#else
int main()
{
    std::cout << "This tutorial code needs the xfeatures2d contrib module to be run." << std::endl;
    return 0;
}
#endif