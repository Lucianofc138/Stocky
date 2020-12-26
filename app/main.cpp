#include <iostream>
#include <string>
#include <opencv2/core/persistence.hpp>

int main( int argc, char* argv[] )
{
    cv::FileStorage data("app/data/data.json", cv::FileStorage::READ);
    cv::FileNode shelves = data["shelves"];
    cv::FileNode prod = shelves[0]["products"][0];
    
    std::string str1 = prod.string();
    if (shelves[0]["id"].string().compare("1")==0)
        std::cout << "hola" << std::endl << std::endl;
    // else
    //     std::cout <<  shelves[0]["id"].string();
    
    // std::cout << shelves[0]["id"] << std::endl << std::endl;
    return 0;
}