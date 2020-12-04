#include "floorFunctions.hpp"

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

ShelfInfo getShelfInfo(cv::Mat image){

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
    imshow("EqGray", src_gray);
    Sobel(src_gray, grad_y, ddepth, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT);
    convertScaleAbs(grad_y, grad_y);
    equalizeHist(grad_y, grad_y);
    imshow("EqY", grad_y);

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

    imshow("First Mask", grad_y);

    struct ShelfInfo shelf_info;
        
    std::vector<int> puntos_linea;
    int count = 0;
    shelf_info.thickness = get_lines_thickness(grad_y);
    int line_counter = 0;
    bool last_was_white = false;

    for (int i = 0; i < grad_y.rows; i++){
        if (grad_y.at<unsigned int>(i, 0) > 128){
            puntos_linea.push_back((int)i);
            last_was_white = true;  
        } else { 
            if (last_was_white){
                shelf_info.centers.push_back( avrg_row(puntos_linea) );
                puntos_linea.clear();
                line_counter++;
            }
            last_was_white = false;
        }
    }

    for (int i=0; i<shelf_info.centers.size()-1; i++){
        int current = shelf_info.centers.at(i);
        int next = shelf_info.centers.at(i+1);

        if ( (next-current)< 3*shelf_info.thickness ){
            shelf_info.centers[i] = (next+current)/2 ;
            shelf_info.centers.erase(shelf_info.centers.begin()+i+1);
        } else {
            shelf_info.centers[i] = current - 0.6*shelf_info.thickness;
            if (shelf_info.centers[i]<0) shelf_info.centers[i] = 0;
        }
    } 
    // ----------------------------------------------------------
    std::cout << "There are " << line_counter << " lines "<< std::endl;

    return shelf_info;
}