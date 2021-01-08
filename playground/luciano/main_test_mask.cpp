#include <iostream>
#include <boost/property_tree/json_parser.hpp>

#include <opencv2/highgui.hpp> //imshow

#include "shelfUtils/shelfUtils.hpp"
#include "processingMisc/processingMisc.hpp"

using namespace cv;
using std::cout;
using std::endl;

int main( int argc, char* argv[] )
{
    std::string path = "masKTest.jpg";
    Mat mask = imread( path, IMREAD_GRAYSCALE);

    Rect roi1(83, 40, 824, 171); 
    Rect roi2(131, 424, 807, 137); 
    imshow("mask", mask);
    imshow("roi1", mask(roi1)); // vacio
    imshow("roi2", mask(roi2)); // no vacio
    Mat img2 = mask(roi1);
    uchar* data = img2.data;
    int maskPix = 0;
    for(int i = 0; i < img2.rows; i++) {
        for(int j = 0; j < img2.cols; j++) {

            // data[i*img2.step + j*img2.channels() + 0]
            // = data[i*img2.step + j*img2.channels() + 1] = 0;
            if (data[i*img2.step + j*img2.channels() + 0] > 128){
                maskPix++;
            }
        }
    }
    if (maskPix > img2.rows*img2.cols*0.1)
        cout << "ajsnfjan" << endl;
    imshow("img2", img2);

    cv::waitKey(0);
    return 0;
}