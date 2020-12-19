#ifndef PERSP_UTILS
#define PERSP_UTILS

#include <opencv2/imgproc.hpp>
#include "opencv2/calib3d.hpp"

// #include "opencv2/imgcodecs.hpp"
// #include "opencv2/videoio.hpp"
// #include "opencv2/highgui.hpp"
// #include <opencv2/opencv.hpp>
// #include "opencv2/xfeatures2d.hpp"
// #include "opencv2/features2d.hpp"
// #include <iostream>

cv::Mat getShelfFrontView(cv::Mat srcImg, cv::Mat perspectiveMat, cv::Size dstSize);
cv::Mat getHomography(cv::Mat frame, std::vector < cv::Point2f > srcPoints, cv::Size dstSize);

#endif