/*
* @file :   "calcShelfMask.cpp"
* @author : Luciano  Flores  Castillo
* @date :   13/11/2020
* @brief :  Código para calcular máscara de los "pisos" de una repisa y separar en un vector
* @comment: Varias de las secciones vienen de la documentación de openCV
            Especificamente la parte de Sobel
*/

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/mat.hpp>   // para clases del Kmeans
#include <opencv2/core/types.hpp> //criteria

#include <iostream>

#include "floorFunctions.hpp"

using namespace cv;
using namespace std;

/* struct ShelfInfo{
    std::vector<int> centers;
    int thickness;
    std::vector<Rect> floors;
};
 */

int avrg_row(std::vector<int> rows);
int get_lines_thickness(Mat img);


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
    Mat image, src, src_gray;
    Mat grad;
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
        
        ShelfInfo shelf_info = getShelfInfo(image);

        cv::Mat mask(image.rows, image.cols, CV_8UC3, cv::Scalar(0,0,0));
        
        for(int i=0; i<shelf_info.centers.size(); i++){
            line(mask, 
                cv::Point(0, shelf_info.centers.at(i)),
                cv::Point(image.cols-1, shelf_info.centers.at(i)), 
                cv::Scalar(255, 255, 255), shelf_info.thickness,
                cv::LINE_8, 0);
        }

        imshow("Mascara", mask);

        int thick = shelf_info.thickness;
        for(int i=0; i < shelf_info.centers.size()-1; i++){
            if (i==0){
                if (shelf_info.centers.at(i) > 2*shelf_info.thickness ){
                    Rect temp_rect(0, thick/2, image.cols, shelf_info.centers.at(i)-shelf_info.thickness);
                    shelf_info.floors.push_back(temp_rect);
                }
            }

            Rect temp_rect(0, shelf_info.centers.at(i) + thick/2, image.cols, 
                           shelf_info.centers.at(i+1) - shelf_info.centers.at(i) - shelf_info.thickness);
            shelf_info.floors.push_back(temp_rect);
        }

        int last_index = shelf_info.centers.size() - 1;

        if ( (mask.rows - shelf_info.centers.at(last_index)) > 2*shelf_info.thickness ){
            int a = image.rows - shelf_info.centers.at( last_index ) - 1;
            Rect temp_rect(0, shelf_info.centers.at( last_index ) + thick/2, image.cols, 
                    image.rows - shelf_info.centers.at( last_index ) - thick/2 - 1);
            shelf_info.floors.push_back(temp_rect);
        }

        Scalar colores[] = { Scalar(0, 0, 255),
                             Scalar(0, 255, 255),
                             Scalar(255, 0, 255),
                             Scalar(0, 255, 0),
                             Scalar(255, 255, 0),
                             Scalar(255, 0, 0) };
        double alpha = 0.5;                     
        for(int i=0; i < shelf_info.floors.size(); i++){
            Mat roi = image(shelf_info.floors.at(i));
            Mat color(roi.size(), CV_8UC3, colores[i]);
            cv::addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
        }

        imshow("Alpha", image);

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

