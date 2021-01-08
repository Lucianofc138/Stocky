#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "perspectiveUtils/perspectiveUtils.hpp"
#include "shelfUtils/shelfUtils.hpp"

// using namespace cv;
// using namespace std;

int main( int argc, char** argv )
{
    cv::CommandLineParser parser(argc, argv,
                                 "{@input   | samples/videos/repisa0.mp4 |input image}"
                                );
    cv::String fileName = parser.get<cv::String>("@input");
    cv::VideoCapture capture(fileName);
    cv::Mat frame;
    if( !capture.isOpened() )
        throw "Error when reading steam_avi";

    std::vector < cv::Point2f > srcPoints;
    srcPoints.push_back(cv::Point2f(48,208));
    srcPoints.push_back(cv::Point2f(448,198));
    srcPoints.push_back(cv::Point2f(429,791));
    srcPoints.push_back(cv::Point2f(54,752));
    cv::Size dstSize(cv::Size(400,500));
    cv::Mat dstImg;
    capture >> frame;
    cv::Mat dstPerspective = getHomography(frame, srcPoints, dstSize);
    cv::resize(frame, frame, cv::Size(0.45*frame.cols, 0.45*frame.rows));
    cv::rotate(frame, frame, cv::ROTATE_90_CLOCKWISE);
    dstImg = getShelfFrontView(frame, dstPerspective, dstSize);
    //cv::imshow("frame", dstImg);

    Shelf shelf1(dstImg, 200);
    shelf1.calcEmptyMask();
    for( ; ; )
    {
        capture >> frame;
        cv::rotate(frame, frame, cv::ROTATE_90_CLOCKWISE);
        if(frame.empty())
            break;
        int cols = frame.cols;
        int rows = frame.rows;
        cv::resize(frame, frame, cv::Size(0.45*cols, 0.45*rows));
        dstImg = getShelfFrontView(frame, dstPerspective, dstSize);
        shelf1.updateImage(dstImg);
        //cv::imshow("video", dstImg);
        cv::waitKey(30);
    }
    cv::waitKey(0);

    return 0;
}