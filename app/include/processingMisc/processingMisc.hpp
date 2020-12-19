#ifndef PROC_MISC_
#define PROC_MISC_

#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

#include <map>
#include <opencv2/imgproc.hpp>




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

    void scanFeaturesSlidingWindow(cv::Mat templateImg, cv::Mat floorImg, std::vector<cv::Rect> boxes);

    // ------------------------ DRAW RESOURCES -------------------------------

    void colorBlobsInImage(cv::Mat& image, cv::Mat& blobsMask, cv::Scalar color );

}




#endif
