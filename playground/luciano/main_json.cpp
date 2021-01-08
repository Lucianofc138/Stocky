#include <iostream>
#include <string>
#include <opencv2/highgui.hpp> 
#include <opencv2/core/persistence.hpp>

int main( int argc, char* argv[] )
{
    cv::FileStorage data("app/data/data.json", cv::FileStorage::READ);
    cv::FileNode products = data["products"];
    cv::FileNode prodType1 = products[0];

    std::string path = prodType1["path"];

    std::string temp_path = path + "cereal0.jpg";
    cv::Mat img = cv::imread(temp_path);
    cv::imshow("cereal", img);
    // std::vector < cv::Mat > images;
    // while(true){

        
    // }
    cv::waitKey(0);
    return 0;
}