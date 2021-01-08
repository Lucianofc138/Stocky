#ifndef PROC_MISC_
#define PROC_MISC_

#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/bgsegm.hpp>

#include <map>




namespace stky
{

    // ------------------------ SCAN FLOOR LIBRARY -------------------------------
    struct MatchInfo
    {
        std::vector<cv::KeyPoint> keypts_temp;
        std::vector<cv::KeyPoint> keypts_roi;
        std::vector<cv::DMatch> good_matches;
    };

    struct RelevantPoints
    {
        std::vector<cv::Point2f> src_pts;
        std::vector<cv::Point2f> dst_pts;
    };

    void scanFeaturesSlidingWindow(cv::Mat templateImg, cv::Mat floorImg, std::vector<cv::Rect> &boxes);

    bool doTheyMatch(cv::Mat tempImg, cv::Mat boxImage, 
                     int numGoodMatch, double distThreshold);
    // ------------------------ DRAW RESOURCES -------------------------------

    void colorBlobsInImage(cv::Mat& image, cv::Mat& blobsMask, cv::Scalar color );

    // ------------------------ MOVEMENT MASK -----------------------------------

    struct BgSustractorInfo
    {
        int historyMOG = 130;
        int nmixtures = 5;
        double backgroundRatio = 0.07;
        double learningRate = 0.005;
        double varThreshold = 25;
        bool bShadowDetection = true;
        cv::Ptr<cv::BackgroundSubtractorMOG2> mog;
    };

    class RelevantFrames
    {
        public:
            RelevantFrames( cv::Mat bg_ = cv::Mat() );

            void initMog();

            cv::Mat getFg();

            void setFrame(cv::Mat frame_);

            void calcFg();
            
        private:
            cv::Mat frame;
            cv::Mat bg;
            cv::Mat fg;
            int frameNumber;
            BgSustractorInfo bgSustractor;

            void getMovementMask();
            
    };

}

#endif
