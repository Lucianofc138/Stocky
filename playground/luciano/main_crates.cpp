#include <iostream>

#include <opencv2/highgui.hpp> //imshow

#include "shelfUtils/shelfUtils.hpp"
#include "processingMisc/processingMisc.hpp"

using namespace cv;
using std::cout;
using std::endl;

int main( int argc, char* argv[] )
{
    std::string path = "app/data/data.json";
    Mat shelfImg = imread( "samples/repisas/cereales/r_cereal0.jpg");
    
    if ( shelfImg.empty() )
    {
        cout << "Could not open or find the image!\n" << endl;
        return -1;
    }
    cv::imshow("Shelf", shelfImg );
    cv::waitKey(0);

    Shelf shelf1(shelfImg, 140, path, true, 1);
        
    Floor floor1 = shelf1.getFloor(2);
    cv::Mat floorImage = shelfImg(floor1.getFloorRect());
    // imshow("Piso 2", floorImage);
    floor1.calcCrates();

    // Mat mask = imread( "masKTest.jpg", IMREAD_GRAYSCALE);
    
    // shelf1.updateFgMask(mask);
    // shelf1.checkIfFgIntersectsFloors();

    cv::waitKey(0);
    return 0;
}