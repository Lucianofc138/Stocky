#ifndef SHELF_UTILS_
#define SHELF_UTILS_


#include <iostream>
#include <vector>

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/mat.hpp> 
#include <opencv2/core/types.hpp> 

class Floor
{ 
    public:
        Floor(cv::Rect rect);
        
        cv::Rect getFloorRect();

        cv::Mat getEmptyMask();

        void calcEmptyMask(cv::Mat src_img);

    private:
        cv::Rect floorRect;
        std::vector<cv::Rect> products;
        cv::Mat emptyMask;

}; 

class Shelf
{ 
    public:
        Shelf(cv::Mat shelfImage);

        void calcShelfInfo(cv::Mat image);

        cv::Mat getShelfMask(int height, int width);

        void fillFloorsVect(int height, int width);

        void paintFloorRects(cv::Mat image);

        void calcEmptyMask(cv::Mat image);

        Floor getFloor(int floor);

    private:
        std::vector<int> centers;
        int thickness;
        std::vector<Floor> floors;
        cv::Mat shelfMask;
        cv::Mat emptyMask;

        void calcShelfMask(int height, int width);

        int avrg_row(std::vector<int> rows);
        
        int get_lines_thickness(cv::Mat img);
        
}; 

#endif