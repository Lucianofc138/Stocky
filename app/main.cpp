#include <iostream>
#include <vector>

#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <opencv2/core/mat.hpp> 
#include <opencv2/core/types.hpp> 

#include "shelfUtils/shelfUtils.hpp"

using namespace cv;
using namespace std;


int main(int argc, char **argv)
{
    cv::CommandLineParser parser(argc, argv,
                                 "{@input   |sample.jpg|input image}"
                                 "{ksize   k|1|ksize (hit 'K' to increase its value at run time)}"
                                 "{scale   s|2|scale (hit 'S' to increase its value at run time)}"
                                 "{help    h|false|show help message}");
    cout << "Mensaje predeterminadoooooooooooooooooooooooooooo\n\n";
    parser.printMessage();
    cout << "\nPress 'ESC' to exit program." << endl
         << "Press 'R' to reset values ( ksize will be -1 equal to Scharr function )" << endl;
    // First we declare the variables we are going to use
    cv::Mat image, src, src_gray;
    cv::Mat grad;
    const String window_name = "Sobel Demo - Simple Edge Detector";
    int ksize = parser.get<int>("ksize");
    int scale = parser.get<int>("scale");
    int delta = 0;
    int ddepth = CV_16S;
    String imageName = parser.get<String>("@input");
    // As usual we load our source image (src)


    

    for (;;){
        image = imread(samples::findFile(imageName), IMREAD_COLOR); // Load an image
        // Check if image is loaded fine
        if (image.empty())
        {
            printf("Error opening image: %s\n", imageName.c_str());
            return EXIT_FAILURE;
        }
        imshow("Original", image);
        // --------------------------------------------------------------------------
        // --------------------------------------------------------------------------

         Shelf shelf1(image);
        shelf1.calcEmptyMask(image);
        
        Floor floor1 = shelf1.getFloor(2);
        cv::Mat floorImage = image(floor1.getFloorRect());
        imshow("Piso 2", floorImage);
        shelf1.paintFloorRects(image);

/*        std::vector<cv::Rect> columns;
        int columns_amount = 20;
        int x = 0, width = floorImage.cols, height = floorImage.rows;
        int col_wid = width/columns_amount;

        for(int i=0; i<columns_amount; i++){
            cv::Rect new_col(x, 0, col_wid-1, height);
            columns.push_back(new_col);
            String win_tag = "";
            win_tag.push_back((char)('0' + i));
            imshow(win_tag, floorImage(new_col));
            x += col_wid;
        } */



        // --------------------------------------------------------------------------
        // --------------------------------------------------------------------------
        char key = (char)waitKey(0);
        if (key == 27)
        {
            return EXIT_SUCCESS;
        }
        else if (key == 'k' || key == 'K')
        {
            ksize = ksize < 30 ? ksize + 2 : 1;
            cout << "ksize: " << ksize << endl;
        }
        else if (key == 's' || key == 'S')
        {
            scale++;
            cout << "scale: " << scale << endl;
        }
        else if (key == 'r' || key == 'R')
        {
            scale = 1;
            ksize = 1;
        }
    }
    return EXIT_SUCCESS;
}
  