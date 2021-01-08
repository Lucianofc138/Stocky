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

class CrateModel
{
    public:
        CrateModel();                     
        // CrateModel(cv::Mat srcImg_, cv::Rect reg_, cv::Mat templateImg_);
        void loadCrate(cv::Mat srcImg_, cv::Rect reg_, cv::Mat templateImg_);
        void updateCrateImg(cv::Mat img_);
        void checkIfImageMatch();

        cv::Mat getCrateImage();
        cv::Mat getTemplateImage();

        void setCrateImage(cv::Mat crateImage_);
        void setTemplateImage(cv::Mat templImage_);

    private:
        void calcHist();
        cv::Mat templateImage;
        cv::Mat crateImage;
        // Hist histogram;
        std::time_t init_time;
        std::time_t finish_time;
};

class Crate
{
    public:
    
        Crate(cv::Mat srcImg_, cv::Rect reg_, cv::Mat prodTmplt);

        void updateCrate(cv::Mat floorImg_);
        void updateCrateImg(cv::Mat floorImg_);
        void checkModels();

        cv::Rect getRect();
        
        cv::Mat getCurrentImage();

        bool isItEmpty();

        bool isItRight();

        void calcIfEmpty();

    private:
        cv::Rect regionInFloor;
        CrateModel currentModel;
        CrateModel rightModel;
        bool productIsRight;
        bool isEmpty;
        std::time_t lastUpdate;
        bool checkedSinceLastUpdate;
        
        void updateStatus();

        void checkCurrentModelTime();

        void updateRightModel();
        

};

class Floor
{ 
    public:
        Floor(cv::Mat shelfImage, cv::Rect rect, cv::Mat fgMask_,std::string path_, 
             std::vector< cv::FileNode > productsNodes_,
             std::vector< std::string > productsList_);

        void updateFloor(cv::Mat shelfImage, cv::Mat shelfFgMask_);

        void updateImage(cv::Mat shelfImage);

        void setFgMask(cv::Mat shelfFgMask_);

        void checkIfFgIntersectsCrates();
        
        bool isMoving();

        void reactToMovementEnd();

        cv::Rect getFloorRect();

        cv::Mat getFloorImage();

        cv::Mat getEmptyMask();

        cv::Mat getFgMask();
        cv::Mat getWrongProductsMask();

        void setProducts( std::vector< std::string > products_ );

        void calcEmptyMask(cv::Mat src_img, int emptyThreshold);

        void calcCrates();

        void calcCrateMask();

        void calcWrongProductMask();

        Crate getCrate(int index);

        void yaBastaFreezer();
        

    private:
        cv::Rect floorRect;
        cv::Mat floorImage;
        std::vector< Crate > crates;
        cv::Mat crateMask;
        cv::Mat emptyMask;
        cv::Mat foregroundMask;
        cv::Mat wrongProductsMask;
        bool moveFlag;
        std::vector< cv::FileNode > productsNodes;
        std::vector< std::string > productsList;
        std::string dataPath;

}; 

class Shelf
{ 
    public:

        Shelf(  cv::Mat shelfImage_ = cv::Mat(), int emptyThreshold=160,
                std::string const jsonPath = "", bool chargeFromJson = false, int id = 0);

        void loadShelfFromJson(std::string const  jsonPath, int id);

        void loadProductsFromJson(std::string const jsonPath, int id);

        void saveJSON();

        void updateShelf(cv::Mat frame_, cv::Mat fg_);

        void updateImage(cv::Mat frame);

        void setFgMask(cv::Mat fgMask_);
        // -----------------------------------------

        void checkIfFgIntersectsFloors();

        void calcShelfInfo(cv::Mat image);

        cv::Mat getShelfMask(int height, int width);

        void fillFloorsVect(int height, int width);

        void calcEmptyMask();

        void calcWrongProductsMask();

        void paintFloorRects(cv::Mat image);

        Floor getFloor(int floor);

        cv::Mat getShelfImage();

        cv::Mat getEmptyMask();

        cv::Mat getFgMask();
        
        cv::Mat getWrongProductMask();
        
        bool isMoving();

        void drawGuiImage(bool drawEmpty, bool drawWrong);

        cv::Mat getGuiImage();
    private:
        int emptyThreshold;

        int thickness;
        std::vector<int> centers;
        std::vector<Floor> floors;
        
        std::vector< std::string > productsList;
        std::vector< cv::FileNode > productsNodes;
        cv::FileNode shelfNode;
        std::string dataPath;

        cv::Mat shelfMask;
        cv::Mat emptyMask;
        cv::Mat shelfImage;
        cv::Mat wrongProductMask;
        cv::Mat foregroundMask;
        cv::Mat guiImage;
        cv::Size imgSize;

        std::time_t last_movement_time;
        bool fgDidMove;

        void calcShelfMask(int height, int width);

        int avrg_row(std::vector<int> rows);
        
        int get_lines_thickness(cv::Mat img);
        
}; 

#endif