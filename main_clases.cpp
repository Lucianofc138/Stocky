#include <iostream>
#include <vector>

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/mat.hpp> 
#include <opencv2/core/types.hpp> 

using namespace cv;
using namespace std;

class Floor
{ 
    public: 
        Floor(cv::Rect rect){
            this->floorRect = rect;
        }
        
        cv::Rect getFloorRect(){
            return this->floorRect;
        }

        cv::Mat getEmptyMask(){
            return this->emptyMask;
        }

        void calcEmptyMask(cv::Mat src_img){
            // cv::Size floorSize = this->floorRect.size();
            // cv::Mat src(floorSize, CV_8UC4);
            cv::Mat src_rect = src_img(this->floorRect);
            cv::Mat gray, thresh, mask;
            cv::cvtColor(src_rect, gray, cv::COLOR_BGR2GRAY);
            equalizeHist(gray, gray);
            threshold(gray, thresh, 150, 255, 0);

            imshow("FloorThresh", thresh);


            cv::Size dilate_size(thresh.cols/50, thresh.rows*2);
            cv::Mat dilate_struct = getStructuringElement(cv::MORPH_RECT, dilate_size);
            dilate(thresh, mask, dilate_struct, cv::Point(-1, -1));

            cv::Size size2(thresh.cols/50, thresh.cols/50);
            cv::Mat struct2 = getStructuringElement(cv::MORPH_RECT, size2);
            erode(mask, mask, struct2, cv::Point(-1, -1), 1, cv::BORDER_ISOLATED);

            bitwise_not(mask, mask);

            imshow("FloorMask", mask);

            this->emptyMask = mask;
        }

    private:
        cv::Rect floorRect;
        std::vector<cv::Rect> products;
        cv::Mat emptyMask;

}; 

class Shelf
{ 
    public:
        Shelf(cv::Mat shelfImage){
            this->calcShelfInfo(shelfImage);
            this->getShelfMask(shelfImage.rows, shelfImage.cols);
            this->fillFloorsVect(shelfImage.rows, shelfImage.cols);
            this->paintFloorRects(shelfImage);
        }

        void calcShelfInfo(cv::Mat image){
            int ksize = 1;
            int scale = 2;
            int delta = 0;
            int ddepth = CV_16S;

            imshow("Original", image);

            cv::Mat src, src_gray, grad_y;
            // Remove noise by blurring with a Gaussian filter ( kernel size = 3 )
            GaussianBlur(image, src, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
            // src = image;
            // Convert the image to grayscale
            cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
            equalizeHist(src_gray, src_gray);
            // imshow("EqGray", src_gray);
            Sobel(src_gray, grad_y, ddepth, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT);
            convertScaleAbs(grad_y, grad_y);
            equalizeHist(grad_y, grad_y);
            // imshow("EqY", grad_y);

            // Structuring elements and their size
            cv::Size hor_erode_size(grad_y.cols / 4, 1);
            cv::Size hor_dilate_size(grad_y.cols * 2, grad_y.rows/100);

            cv::Mat hor_erode_structure = getStructuringElement(cv::MORPH_RECT, hor_erode_size);
            cv::Mat hor_dilate_structure = getStructuringElement(cv::MORPH_RECT, hor_dilate_size);

            // Apply morphology operations
            threshold(grad_y, grad_y, 224, 255, 0);
            // imshow("Thresh", grad_y);
            erode(grad_y, grad_y, hor_erode_structure, cv::Point(-1, -1));
            // imshow("After Erode", grad_y);
            dilate(grad_y, grad_y, hor_dilate_structure, cv::Point(-1, -1));

            // imshow("First Mask", grad_y);

            std::vector<int> puntos_linea;
            int count = 0;
            this->thickness = get_lines_thickness(grad_y);
            int line_counter = 0;
            bool last_was_white = false;

            for (int i = 0; i < grad_y.rows; i++){
                if (grad_y.at<unsigned int>(i, 0) > 128){
                    puntos_linea.push_back((int)i);
                    last_was_white = true;  
                } else { 
                    if (last_was_white){
                        this->centers.push_back( avrg_row(puntos_linea) );
                        puntos_linea.clear();
                        line_counter++;
                    }
                    last_was_white = false;
                }
            }
            for (int i=0; i<this->centers.size()-1; i++){
                int current = this->centers.at(i);
                int next = this->centers.at(i+1);

                if ( (next-current)< 3*this->thickness ){
                    this->centers[i] = (next+current)/2 ;
                    this->centers.erase(this->centers.begin()+i+1);
                } else {
                    this->centers[i] = current - 0.6*this->thickness;
                    if (this->centers[i]<0) this->centers[i] = 0;
                }
            } 
            // ----------------------------------------------------------
            std::cout << "There are " << line_counter << " lines "<< std::endl;

        }

        cv::Mat getShelfMask(int height, int width){
            calcShelfMask(height, width);
            return this->shelfMask;
        }

        void fillFloorsVect(int height, int width){
            int thick = this->thickness;
            for(int i=0; i < this->centers.size()-1; i++){
                if (i==0){ // Primer Rect (si hay suficiente espacio)
                    if (this->centers.at(i) > 2*thick ){
                        cv::Rect temp_rect(0, thick/2, width, this->centers.at(i)-thick);
                        Floor temp_floor(temp_rect);
                        this->floors.push_back(temp_floor);
                    }
                }
                // Rect entre medio

                cv::Rect temp_rect(0, this->centers.at(i) + thick/2, width, 
                            this->centers.at(i+1) - this->centers.at(i) - thick);
                Floor temp_floor(temp_rect);
                this->floors.push_back(temp_floor);
            }

            int last_index = this->centers.size() - 1;
            
            // Último rect (si hay espacio)
            if ( (height - this->centers.at(last_index)) > 4*this->thickness ){
                int a = height - this->centers.at( last_index ) - 1;
               cv::Rect temp_rect(0, this->centers.at( last_index ) + thick/2, width, 
                        height - this->centers.at( last_index ) - thick/2 - 1);
                Floor temp_floor(temp_rect);
                this->floors.push_back(temp_floor);
            }
        }

        void paintFloorRects(cv::Mat image){
            int height = image.rows;
            int width = image.cols;

            cv::Mat floorRectsImage = image.clone();

            cv::Scalar colores[] = {
                cv::Scalar(0, 0, 255),
                cv::Scalar(0, 255, 255),
                cv::Scalar(255, 0, 255),
                cv::Scalar(0, 255, 0),
                cv::Scalar(255, 255, 0),
                cv::Scalar(255, 0, 0) };

            double alpha = 0.5;

             for(int i=0; i < (this->floors.size()); i++){
                Floor temp_floor = this->floors.at(i);
                cv::Rect roi = temp_floor.getFloorRect();
                cv::Mat roiMat = floorRectsImage(roi);
                cv::Mat color(roi.size(), CV_8UC3, colores[i]);
                cv::addWeighted(color, alpha, roiMat, 1.0 - alpha , 0.0, roiMat);
            }

            // imshow("Alpha", floorRectsImage);

        }

        void calcEmptyMask(cv::Mat image){
            cv:: Mat emptyMask( image.rows, image.cols, CV_8UC1, cv::Scalar(0,0,0) );
            this->emptyMask = emptyMask.clone();

            for(int i; i<(this->floors.size()); i++){
                this->floors.at(i).calcEmptyMask(image);
                cv::Rect floorRect = this->floors.at(i).getFloorRect();
                cv::Mat floorMask = this->floors.at(i).getEmptyMask();
                floorMask.copyTo(this->emptyMask(floorRect));
            }   
            imshow("Máscara Espacios Vacío", this->emptyMask);
        }

        Floor getFloor(int floor){
            return this->floors.at(floor);
        }
    private:
        std::vector<int> centers;
        int thickness;
        std::vector<Floor> floors;
        cv::Mat shelfMask;
        cv::Mat emptyMask;

        void calcShelfMask(int height, int width){
            cv::Mat mask(height, width, CV_8UC3, cv::Scalar(0,0,0));
        
            for(int i=0; i<this->centers.size(); i++){
                line(mask, 
                    cv::Point(0, this->centers.at(i)),
                    cv::Point(width-1, this->centers.at(i)), 
                    cv::Scalar(255, 255, 255), this->thickness,
                    cv::LINE_8, 0);
            }

            imshow("Mascara", mask);
        }

        int avrg_row(std::vector<int> rows){
            int avrg = 0;
            int len = rows.size();
            for(int i = 0; i < len; i++){
                int val = rows.back();
                avrg += rows.back();
                rows.pop_back();
                
            }
            avrg /= len;
            return avrg;
        }
        
        int get_lines_thickness(cv::Mat img){
            int line_counter = 0;
            bool last_was_white = false;
            for (int i = 0; i < img.rows; i++){
                    if (img.at<unsigned int>(i, 0) > 128){

                        last_was_white = true;  
                    } else { 
                        if (last_was_white){
                            line_counter++;
                        }
                        last_was_white = false;
                    }
                }
            int thickness = 0.3*(img.rows/line_counter);
            return thickness;
        }
        
}; 



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
        // shelf1.calcEmptyMask(image);
        
        Floor floor1 = shelf1.getFloor(2);
        cv::Mat floorImage = image(floor1.getFloorRect());
        imshow("Piso 2", floorImage);

        std::vector<cv::Rect> columns;
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
        }



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
  