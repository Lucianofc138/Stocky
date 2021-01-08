#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "processingMisc/processingMisc.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    stky::RelevantFrames pictures;
    pictures.initMog();
	
	cv::VideoCapture capture;
    capture.open(0);
	if (!capture.isOpened())
		throw "Error opening camara";
	cv::Mat frame;
	for (;;)
	{
		capture >> frame;
		if (frame.empty())
			break;
		cv::resize(frame, frame, cv::Size(0.45 * frame.cols, 0.45 * frame.rows));
		pictures.setFrame(frame);
		pictures.calcFg();
        cv::imshow("Video", frame);
		if (!pictures.getFg().empty())
			cv::imshow("Mask", pictures.getFg());
		cv::waitKey(30);
	}
	cv::waitKey(0);
	return 0;
}