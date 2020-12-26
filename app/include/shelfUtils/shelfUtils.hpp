#ifndef SHELF_UTILS_
#define SHELF_UTILS_

#include <iostream>
#include <ctime>  // timestamp

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/mat.hpp> 
#include <opencv2/core/types.hpp>
#include <opencv2/core/persistence.hpp> // JSON

#include "histUtils/histUtils.hpp"
#include "processingMisc/processingMisc.hpp"

// class CrateModel
// {
//     public:
//         CrateModel(cv::Mat srcImg_, cv::Rect reg_, cv::Mat templateImg_);

        



//     private:
//         void calcHist();
//         cv::Mat templateImage;
//         cv::Mat crateImage;
//         Hist histogram;
//         std::time_t init_time;
//         std::time_t finish_time;
// };

class Crate
{
    public:
        Crate(cv::Mat srcImg_, cv::Rect reg_);

    private:
        cv::Rect regionInFloor;
        // CrateModel currentModel;
        // CrateModel rightModel;
        bool productIsRight;
        bool isEmpty;
        
        void updateStatus(); // compara producto actual con correcto y actualiza

        void checkCurrentModelTime();

        void updateRightModel();
        

};

class Floor
{ 
    public:
        Floor(cv::Mat shelfImage, cv::Rect rect);

        void updateImage(cv::Mat shelfImage);
        
        cv::Rect getFloorRect();

        cv::Mat getFloorImage();

        cv::Mat getEmptyMask();

        void setProducts( std::vector< std::string > products_ );

        void calcEmptyMask(cv::Mat src_img, int emptyThreshold);

        void calcCrates();

    private:
        cv::Rect floorRect;
        cv::Mat floorImage;
        std::vector< Crate > products;
        cv::Mat emptyMask;
        cv::Mat foregroundMask;
        std::vector< std::string > products;

}; 

class Shelf
{ 
    public:

        Shelf(  cv::Mat shelfImage = cv::Mat(), int emptyThreshold=160,
                std::string jsonPath = "", int id = 0    );

        // Shelf(std::string jsonPath, int id);

        void loadJSON(std::string jsonPath, int id);

        void updateImage(cv::Mat frame);

        // -----------------------------------------

        void calcShelfInfo(cv::Mat image);

        cv::Mat getShelfMask(int height, int width);

        void fillFloorsVect(int height, int width);

        void calcEmptyMask();

        void paintFloorRects(cv::Mat image);

        Floor getFloor(int floor);

    private:
        int emptyThreshold;

        int thickness;
        std::vector<int> centers;
        std::vector<Floor> floors;
        std::vector< std::string > products;
        cv::Mat shelfMask;
        cv::Mat emptyMask;
        cv::Mat shelfImage;
        cv::Mat foregroundMask;
        cv::Size imgSize;

        void calcShelfMask(int height, int width);

        int avrg_row(std::vector<int> rows);
        
        int get_lines_thickness(cv::Mat img);
        
}; 

#endif