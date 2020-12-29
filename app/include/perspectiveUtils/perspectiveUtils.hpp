#ifndef PERSP_UTILS
#define PERSP_UTILS

#include <opencv2/imgproc.hpp>
#include "opencv2/calib3d.hpp"


cv::Mat getShelfFrontView(cv::Mat srcImg, cv::Mat perspectiveMat, cv::Size dstSize);
cv::Mat getHomography(cv::Mat frame, std::vector < cv::Point2f > srcPoints, cv::Size dstSize);

#endif