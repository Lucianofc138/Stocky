#include <iostream>
#include <boost/property_tree/json_parser.hpp>

#include <opencv2/highgui.hpp> //imshow

#include "shelfUtils/shelfUtils.hpp"
#include "processingMisc/processingMisc.hpp"

using namespace cv;
using std::cout;
using std::endl;

int main( int argc, char* argv[] )
{
    

    Mat tmpltImg = imread( "samples/filas/prod_samp4/S4F2P5.jpg");
    Mat shelfImg = imread( "samples/repisas/cereales/r_cereal0.jpg");
    
    if ( tmpltImg.empty() || shelfImg.empty() )
    {
        cout << "Could not open or find the image!\n" << endl;
        return -1;
    }
    // cv::imshow("Template", tmpltImg );
    // cv::imshow("Shelf", shelfImg );
    // cv::waitKey(0);

    Shelf shelf1(shelfImg);
    shelf1.calcEmptyMask();
        
    Floor floor1 = shelf1.getFloor(1);
    cv::Mat floorImage = shelfImg(floor1.getFloorRect());
    imshow("Piso 2", floorImage);

    // std::vector<cv::Rect> boxes;
    // stky::scanFeaturesSlidingWindow(tmpltImg, floorImage, boxes);

    cv::waitKey(0);
    return 0;
}
