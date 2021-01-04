#include "shelfUtils/shelfUtils.hpp"

// ----------------------------------------------------------------------------
// -- FLOOR CLASS METHODS DEFINITION ------------------------------------------
// ----------------------------------------------------------------------------

Floor::Floor(cv::Mat shelfImage, cv::Rect rect, cv::Mat fgMask_, std::string path_,  
             std::vector< cv::FileNode > productsNodes_,
             std::vector< std::string > productsList_)
{
    this->floorRect = rect;
    this->floorImage = shelfImage(rect);
    this->productsNodes = productsNodes_;
    this->productsList = productsList_;
    this->dataPath = path_;
    this->foregroundMask = fgMask_;
    //this->calcCrates();
}

void Floor::updateImage(cv::Mat shelfImage){
    this->floorImage = shelfImage(this->floorRect);
}   

void Floor::setFgMask(cv::Mat fg_){
    this->foregroundMask = fg_(this->floorRect);
}

void Floor::checkIfFgIntersectsCrates(){
    int diameter = (this->foregroundMask.cols)/(10); 
    cv::Size size(diameter, diameter);
    cv::Mat structEl = getStructuringElement(cv::MORPH_ELLIPSE, size);
    cv::Mat dilatedFg;
    cv::dilate(this->foregroundMask, dilatedFg, structEl, cv::Point(-1, -1));

    for(int k = 0; k < this->crates.size(); k++)
    {
        Crate currCrate = this->crates.at(k);
        cv::Rect crateRect = currCrate.getRect();
        cv::Mat crateDilMask = dilatedFg(floorRect);
        uchar* data = crateDilMask.data;
        int maskPix = 0;
        for(int i = 0; i < crateDilMask.rows; i++) {
            for(int j = 0; j < crateDilMask.cols; j++) {
            if (data[i*crateDilMask.step + j*crateDilMask.channels() + 0] > 128)
                maskPix++;
            }
            if (maskPix > crateDilMask.rows*crateDilMask.cols*0.05){
                currCrate.updateCrateImg(this->floorImage(crateRect));
                break;
            }
        }
    }
}

cv::Rect Floor::getFloorRect()
{
    return this->floorRect;
}

cv::Mat Floor::getFloorImage()
{
    return this->floorImage;
}

cv::Mat Floor::getEmptyMask()
{
    return this->emptyMask;
}

void Floor::setProducts( std::vector< std::string > products_ ){
    // for(int i=0; i<products_.size(); i++){
    //     products.push_back(products_.at(i));
    // }
}

void Floor::calcEmptyMask(cv::Mat src_img, int emptyThreshold)
{
    // cv::Size floorSize = this->floorRect.size();
    // cv::Mat src(floorSize, CV_8UC4);
    cv::Mat src_rect = src_img(this->floorRect);
    cv::Mat gray, thresh, mask;
    cv::cvtColor(src_rect, gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(gray, gray);
    cv::threshold(gray, thresh, emptyThreshold, 255, 0);

    // cv::imshow("FloorThresh", thresh);

    cv::Size erode_size1(thresh.cols / 200, thresh.rows / 15 );
    cv::Mat erode_struct1 = getStructuringElement(cv::MORPH_RECT, erode_size1);
    cv::erode(thresh, mask, erode_struct1, cv::Point(-1, -1));

    // cv::imshow("mask1", mask);

    cv::Size dilate_size1(thresh.cols / 40, thresh.rows * 2);
    cv::Mat dilate_struct1 = getStructuringElement(cv::MORPH_RECT, dilate_size1);
    cv::dilate(mask, mask, dilate_struct1, cv::Point(-1, -1));

    // cv::imshow("mask2", mask);

    cv::Size size2(thresh.cols / 25, 1);
    cv::Mat dilate_struct2 = getStructuringElement(cv::MORPH_RECT, size2);
    cv::dilate(mask, mask, dilate_struct2, cv::Point(-1, -1));

    // cv::imshow("mask3", mask);
    
    cv::Mat erode_struct2 = getStructuringElement(cv::MORPH_RECT, size2);
    cv::erode(mask, mask, erode_struct2, cv::Point(-1, -1), 1, cv::BORDER_ISOLATED);

    // cv::imshow("mask4", mask);

    cv::bitwise_not(mask, mask);

    // cv::imshow("FloorMask", mask);
    // cv::waitKey(0);

    this->emptyMask = mask;
}

void Floor::calcCrates()
{
    cv::FileStorage json = cv::FileStorage(this->dataPath, cv::FileStorage::READ);    
    cv::FileNode allProducts = json["products"];

    for (int i = 0; i<allProducts.size(); i++){
        cv::FileNode currentProduct = allProducts[i];
        for(int j=0; j<productsList.size(); j++){
            std::string name1 = currentProduct["name"].string();
            std::string nameInList = productsList.at(j);
            if ( name1.compare(nameInList) == 0){
                std::string commonPath = currentProduct["path"].string();
                std::string name = currentProduct["name"].string();
                for(int k=0; k< 24; k++){
                    std::string num = std::to_string(k);
                    std::string path = commonPath + name + num +".jpg";
                    cv::Mat templtImg = cv::imread(path);
                    std::vector<cv::Rect> boxes;
                    cv::imshow("template", templtImg);
                    cv::imshow("floor", floorImage);
                    cv::waitKey(0);
                    stky::scanFeaturesSlidingWindow(templtImg, floorImage, boxes);
                    for (int l=0; l<boxes.size(); l++){
                        Crate tmpCrate(this->floorImage, boxes.at(l), templtImg);
                        this->crates.push_back(tmpCrate);
                    }
                    // cv::waitKey(0);
                    if (boxes.size() > 0)
                        boxes.clear();
                }
                break;
            }
        }
    }
}

void Floor::yaBastaFreezer(){
    std::cout << "Detengan el semestre que me quiero bajar :'(" << std::endl;
}