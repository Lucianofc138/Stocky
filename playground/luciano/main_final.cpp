#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "perspectiveUtils/perspectiveUtils.hpp"
#include "processingMisc/processingMisc.hpp"
#include "shelfUtils/shelfUtils.hpp"

// using namespace cv;
// using namespace std;

int main( int argc, char** argv )
{
    cv::String fileName = "samples/videos/video1.mp4";
    cv::VideoCapture capture(fileName);
    cv::Mat frame;
    if( !capture.isOpened() )
        throw "Error when reading steam_avi";

    std::vector < cv::Point2f > srcPoints;
    srcPoints.push_back(cv::Point2f(372*0.45,111*0.45));
    srcPoints.push_back(cv::Point2f(911*0.45,191*0.45));
    srcPoints.push_back(cv::Point2f(908*0.45,1130*0.45));
    srcPoints.push_back(cv::Point2f(366*0.45,1140*0.45));
    cv::Size dstSize(cv::Size(400,500));
    cv::Mat dstImg;
    capture >> frame;
    cv::Mat dstPerspective = getHomography(frame, srcPoints, dstSize);
    cv::resize(frame, frame, cv::Size(0.45*frame.cols, 0.45*frame.rows));
    dstImg = getShelfFrontView(frame, dstPerspective, dstSize);
    cv::imshow("first frame", dstImg);

    Shelf shelf1(dstImg, 200, "app/data/data2.json", true, 1);
    // shelf1.calcEmptyMask();
    cv::imshow("shelf mask", shelf1.getShelfMask(dstImg.rows, dstImg.cols));

    // stky::RelevantFrames pictures;
    // pictures.initMog();
    // for( ; ; )
    // {
    //     capture >> frame;
    //     // cv::rotate(frame, frame, cv::ROTATE_90_CLOCKWISE);
    //     if(frame.empty())
    //         break;
    //     int cols = frame.cols;
    //     int rows = frame.rows;

    //     cv::resize(frame, frame, cv::Size(0.45*cols, 0.45*rows));
    //     cv::imshow("Original", frame);
    //     dstImg = getShelfFrontView(frame, dstPerspective, dstSize);
    //     shelf1.updateImage(dstImg);
    //     cv::imshow("Perpective", dstImg);
    //     //----------------------------------------
    //     pictures.setFrame(dstImg);
	// 	pictures.calcFg();
	// 	if (!pictures.getFg().empty())
	// 		cv::imshow("Mask", pictures.getFg());
    //     cv::waitKey(30);
    // }
    cv::waitKey(0);

    return 0;
}