#include<iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "processingMisc/processingMisc.hpp"

using namespace std;


int main(int argc, char *argv[]) {
	/* Capturo Camara */
	cv::VideoCapture vid;
	vid.open(0);

	if (!vid.isOpened()) {
		cerr << "Error opening input." << endl;
		return 1;
	}
    cv::Mat img, gray, thresh;
    while(true)
    {
        vid>>img;
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
        cv::threshold(gray, thresh, 128, 255, 0);
        cv::Mat el = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(11, 11));
        cv::erode(thresh, thresh, el);
        cv::dilate(thresh, thresh, el);
        
        imshow("video", thresh);

        stky::colorBlobsInImage(img, thresh, cv::Scalar(255, 0, 0));
        imshow("video", img);
        cv::waitKey(30);
        
    }
    return 0;
}