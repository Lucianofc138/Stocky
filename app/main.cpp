#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utility.hpp> // Cmd Line Parser

#include "perspectiveUtils/perspectiveUtils.hpp"
#include "processingMisc/processingMisc.hpp"
#include "shelfUtils/shelfUtils.hpp"

// using namespace cv;
// using namespace std;

std::vector < cv::Point2f > initHomographyPts();

int main( int argc, char** argv )
{   
    cv::String keys =
        "{@video |samples/videos/video6.mp4 | input video path}" 
        "{@json  |app/data/data3.json       | json path}"
        "{help   |     | usage:  your/video/path  your/json/path}"; 
    cv::CommandLineParser parser(argc, argv, keys);
    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }
    cv::String videoPath = parser.get<cv::String>(0);
    cv::String jsonPath = parser.get<cv::String>(1);
    cv::VideoCapture capture(videoPath);
    cv::Mat frame;
    if( !capture.isOpened() )
        throw "Error when reading steam_avi";

    for (int i=0; i< 901; i++){ 
        capture >> frame;
    }
    
    std::vector < cv::Point2f > srcPoints = initHomographyPts();
    cv::Size dstSize(cv::Size(420,700));
    cv::Mat dstPerspective = getHomography(frame, srcPoints, dstSize);
    cv::Mat dstImg = getShelfFrontView(frame, dstPerspective, dstSize);
    // cv::imshow("First Frame", dstImg);
    // cv::waitKey(0);
    Shelf shelf1(dstImg, 168, jsonPath, true, 1);

    stky::RelevantFrames relevantFrames;
    relevantFrames.initMog();

    for( ; ; )
    {
        capture >> frame;
        if(frame.empty())
            break;
        dstImg = getShelfFrontView(frame, dstPerspective, dstSize);

        relevantFrames.setFrame(dstImg);
		relevantFrames.calcFg();

        shelf1.updateShelf(dstImg, relevantFrames.getFg());

        shelf1.drawGuiImage(true, true);
        cv::imshow("GUI", shelf1.getGuiImage());

        cv::waitKey(1);
    }
    cv::waitKey(0);

    return 0;
}

std::vector < cv::Point2f > initHomographyPts(){
    std::vector < cv::Point2f > srcPoints;
    // ------------ Videos Luciano
    // video 6
    srcPoints.push_back(cv::Point2f(97, 120));
    srcPoints.push_back(cv::Point2f(604, 159));
    srcPoints.push_back(cv::Point2f(615, 1063));
    srcPoints.push_back(cv::Point2f(70, 1088));
    return srcPoints;
}
