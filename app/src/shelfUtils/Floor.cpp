#include "shelfUtils/shelfUtils.hpp"
// #include <opencv2/opencv.hpp>
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
    this->crateMask = cv::Mat::zeros(floorImage.size(), CV_8UC1);
    this->calcCrates();
    this->moveFlag = false;
}

void Floor::updateImage(cv::Mat shelfImage){
    this->floorImage = shelfImage(this->floorRect);
}   

void Floor::setFgMask(cv::Mat shelfFgMask_){
    int max_x = floorRect.x+floorRect.width;
    int max_y = floorRect.y+floorRect.height;
    if( max_x <= shelfFgMask_.cols && max_y <= shelfFgMask_.rows)
        this->foregroundMask = shelfFgMask_(this->floorRect);
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

cv::Mat Floor::getFgMask(){
    return this->foregroundMask;
}

void Floor::reactToMovementEnd()
{
    
    for(int i=0; i<crates.size(); i++){
        // std::cout << "Crate " << i << std::endl;
        crates.at(i).updateCrate(this->floorImage);
    }
}

bool Floor::isMoving(){
    cv::Mat M = this->foregroundMask;
    int i, j, cols = M.cols,
    rows = M.rows;
    int depth = M.depth(),
    channels = M.channels(),
    step = M.step;
    uchar *data = M.data;
    int movePix = 0;
    for(i = 0; i < rows; i++) {
        for(j = 0; j < cols; j++) {
             if (data[i*step + j*channels + 0] >=128)
                movePix++;
        }
    }
    if (movePix > cols*rows*0.05){
        return true;
    }
    else
        return false;
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

    cv::medianBlur(thresh, thresh, 9);
    // cv::imshow("FloorThresh", thresh);
    // cv::waitKey(0);

    mask = thresh;
    
    // cv::Size erode_size0(1, thresh.rows/8);
    // cv::Mat erode_struct0 = getStructuringElement(cv::MORPH_RECT, erode_size0);
    // cv::erode(mask, mask, erode_struct0, cv::Point(-1, -1));

    // cv::imshow("mask0z", mask);
    // cv::waitKey(0);
    
    cv::Size dilate_size0(thresh.cols/15, thresh.rows/4);
    cv::Mat dilate_struct0 = getStructuringElement(cv::MORPH_RECT, dilate_size0);
    cv::dilate(mask, mask, dilate_struct0, cv::Point(-1, -1));

    // cv::imshow("mask0a", mask);
    // cv::waitKey(0);

    // cv::Mat flood; mask.copyTo(flood);
    // uchar *data = flood.data;
    // int step = flood.step;
    // int channels = flood.channels();
    // cv::Point2d floidPt(0,0);
    // bool flag = false;
    // for(int i = 0; i < flood.rows; i++) { //Elegir un punto negro para pintar con flood
    //     for(int j = 0; j < flood.cols; j++) {
    //         if ( data[i*step + j*channels + 0] < 128 ){
    //             floidPt = cv::Point2d(i, j);
    //             flag = true;
    //         }
    //         if (flag) break;
    //     }
    //     if (flag) break;
    // }
    // cv::floodFill(flood, cv::Point2d(200,20), cv::Scalar(255));
    // cv::bitwise_not(flood, flood);
    // cv::imshow("Flood", flood);
    // cv::bitwise_or(flood, thresh, mask);
    // cv::imshow("mask0b", mask);
    // cv::waitKey(0);

    cv::Size erode_size1(thresh.cols / 5, thresh.rows / 1.75 );
    cv::Mat erode_struct1 = getStructuringElement(cv::MORPH_RECT, erode_size1);
    cv::erode(thresh, mask, erode_struct1, cv::Point(-1, -1));

    // cv::imshow("mask1", mask);
    // cv::waitKey(0);

    cv::Size dilate_size1(thresh.cols / 4, thresh.rows * 2);
    cv::Mat dilate_struct1 = getStructuringElement(cv::MORPH_RECT, dilate_size1);
    cv::dilate(mask, mask, dilate_struct1, cv::Point(-1, -1));

    // cv::imshow("mask2", mask);
    // cv::waitKey(0);
    
    cv::Size erode_size2(thresh.cols / 8, 10);
    cv::Mat erode_struct2 = getStructuringElement(cv::MORPH_RECT, erode_size2);
    cv::erode(mask, mask, erode_struct2, cv::Point(-1, -1), 1, cv::BORDER_ISOLATED);

    // cv::imshow("mask4", mask);
    // cv::waitKey(0);

    cv::bitwise_not(mask, mask);

    // cv::imshow("FloorMask", mask);
    // cv::waitKey(0);
    if ( ~this->crateMask.empty() ) // Corrección crates
    {
        calcCrateMask();
        cv::Mat finalMask;
        cv::bitwise_and(mask, this->crateMask, finalMask);
        cv::bitwise_not(finalMask, finalMask);
        cv::bitwise_and(finalMask, mask, finalMask);
        mask = finalMask;
    }

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
                for(int k=0; k< 7; k++){
                    std::string num = std::to_string(k);
                    std::string path = commonPath + name + num +".jpg";
                    cv::Mat templtImg = cv::imread(path);
                    std::vector<cv::Rect> boxes;
                    // cv::imshow("template", templtImg);
                    // cv::imshow("floor", floorImage);
                    // cv::waitKey(0);
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
    calcCrateMask();

}

void Floor::calcCrateMask(){
    this->crateMask = cv::Mat::zeros(floorImage.size(), CV_8UC1);
    for(int k=0; k<crates.size(); k++){
        if (crates.at(k).isItEmpty() && !crates.at(k).isItRight())
            continue;
        cv::Rect rect = crates.at(k).getRect();
        cv::Mat roi = this->crateMask(rect);
        int i, j, cols = roi.cols,
        rows = roi.rows;
        int depth = roi.depth(),
        channels = roi.channels(),
        step = roi.step;
        uchar *data = roi.data;
        for(i = 0; i < rows; i++) {
            for(j = 0; j < cols; j++) {
                data[i*step + j*channels + 0] = 255;
            }
        }
    }
    cv::Size dilate_size(this->crateMask.cols * 0.15, this->crateMask.rows * 2);
    cv::Mat dilate_struct = getStructuringElement(cv::MORPH_RECT, dilate_size);
    cv::dilate(this->crateMask, this->crateMask, dilate_struct, cv::Point(-1, -1));

    cv::Size erode_size(this->crateMask.cols * 0.15, 1);
    cv::Mat erode_struct = getStructuringElement(cv::MORPH_RECT, erode_size);
    cv::erode(this->crateMask, this->crateMask, erode_struct, cv::Point(-1, -1));

    // cv::imshow("CrateMask", this->crateMask);
    // cv::waitKey(0);
}

void Floor::calcWrongProductMask()
{
    this->wrongProductsMask = cv::Mat(this->floorImage.rows,
                                     this->floorImage.cols,
                                     CV_8UC1, cv::Scalar(0, 0, 0));
    for( int i=0; i < crates.size(); i++){
        // crates.at(i).calcIfEmpty();
        if(!crates.at(i).isItRight() && !crates.at(i).isItEmpty()){
            cv::rectangle(this->wrongProductsMask,
                      crates.at(i).getRect(),
                      cv::Scalar(255,255,255), -1);
        }
        
    }
}

void Floor::updateFloor(cv::Mat shelfImage, cv::Mat shelfFgMask_){
    this->updateImage(shelfImage);
    this->setFgMask(shelfFgMask_); 
}

Crate Floor::getCrate(int index){
    return this->crates.at(index);
}

cv::Mat Floor::getWrongProductsMask(){
    return this->wrongProductsMask;
}

void Floor::yaBastaFreezer(){
    std::cout << "Detengan el semestre que me quiero bajar :'(" << std::endl;
}