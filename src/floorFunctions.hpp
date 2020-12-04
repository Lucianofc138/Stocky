#ifndef COMMAND_LINE_ARGUMENTS_TOOLS_HPP_
#define COMMAND_LINE_ARGUMENTS_TOOLS_HPP_

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/mat.hpp>   // para clases del Kmeans
#include <opencv2/core/types.hpp> //criteria

#include <iostream>

struct ShelfInfo{
    std::vector<int> centers;
    int thickness;
    std::vector<cv::Rect> floors;
};

ShelfInfo getShelfInfo(cv::Mat image);

#endif