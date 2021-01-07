#include <perspectiveUtils/perspectiveUtils.hpp>

cv::Mat getShelfFrontView(cv::Mat srcImg, cv::Mat perspectiveMat, cv::Size dstSize){
    cv::Mat dstImg(dstSize, CV_8UC3);
    cv::warpPerspective( srcImg, dstImg, perspectiveMat, dstSize, cv::INTER_LINEAR, cv::BORDER_CONSTANT);
    return dstImg;
}

cv::Mat getHomography(cv::Mat frame, std::vector < cv::Point2f > srcPoints, cv::Size dstSize){
    std::vector < cv::Point2f > dstPoints;
    dstPoints.push_back(cv::Point2f(0,0));
    dstPoints.push_back(cv::Point2f(dstSize.width - 1 ,0));
    dstPoints.push_back(cv::Point2f(dstSize.width - 1,dstSize.height - 1));
    dstPoints.push_back(cv::Point2f(0,dstSize.height - 1));
    cv::Mat perspectiveMat = cv::findHomography(srcPoints,dstPoints);
    return  perspectiveMat;
}