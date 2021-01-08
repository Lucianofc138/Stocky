#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "perspectiveUtils/perspectiveUtils.hpp"
#include "processingMisc/processingMisc.hpp"
#include "shelfUtils/shelfUtils.hpp"

// using namespace cv;
// using namespace std;

std::vector < cv::Point2f > initHomographyPts();

int main( int argc, char** argv )
{
    cv::String fileName = "samples/videos/video6.mp4";
    cv::VideoCapture capture(fileName);
    cv::Mat frame;
    if( !capture.isOpened() )
        throw "Error when reading steam_avi";

    for (int i=0; i< 901; i++){ 
        capture >> frame;
    }
    // cv::imshow("first frame", frame);
    // cv::imwrite("frame0.jpg", frame);
    // cv::waitKey(0);
    std::vector < cv::Point2f > srcPoints = initHomographyPts();
    cv::Size dstSize(cv::Size(420,700));
    cv::Mat dstPerspective = getHomography(frame, srcPoints, dstSize);
    cv::Mat dstImg = getShelfFrontView(frame, dstPerspective, dstSize);

    // cv::imshow("first frame front view", dstImg);
    // cv::imwrite("dstImg.jpg", dstImg);
    Shelf shelf1(dstImg, 168, "app/data/data3.json", true, 1);
    // // shelf1.calcEmptyMask();
    // std::cout << "LISTO?" << std::endl;
    // cv::waitKey(0);

    stky::RelevantFrames relevantFrames;
    relevantFrames.initMog();
    for( int p = 0; ;p++ )
    {
        capture >> frame;
        if(frame.empty())
            break;
        dstImg = getShelfFrontView(frame, dstPerspective, dstSize);
        
        // shelf1.updateImage(dstImg);
        // cv::imshow("Video", shelf1.getShelfImage());
        shelf1.calcEmptyMask();
        // cv::imshow("Empty Mask", shelf1.getEmptyMask());
        shelf1.calcWrongProductsMask();
        // cv::Mat wrong = shelf1.getWrongProductMask();
        // if(!wrong.empty())
        //     cv::imshow("wrong", wrong);
        shelf1.drawGuiImage(true, true);
        cv::imshow("GUI", shelf1.getGuiImage());
        relevantFrames.setFrame(dstImg);
		relevantFrames.calcFg();

        shelf1.updateShelf(dstImg, relevantFrames.getFg());
        
        // shelf1.updateImage(dstImg);
        // shelf1.setFgMask(relevantFrames.getFg());
		// if (!relevantFrames.getFg().empty()){
		// 	cv::imshow("Movement Mask", shelf1.getFgMask());
        // }
        // cv::Mat crateImg = shelf1.getFloor(2).getCrate(0).getCurrentImage();
        // if (!crateImg.empty())
		// 	cv::imshow("Crate Image", crateImg);
        // if (shelf1.isMoving()){
        //     std::cout << "Movement Detected" << p << std::endl;

        // }
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
