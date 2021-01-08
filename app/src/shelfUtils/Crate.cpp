#include "shelfUtils/shelfUtils.hpp"

// ----------------------------------------------------------------------------
// -- CRATE MODEL CLASS METHODS DEFINITION ------------------------------------
// ----------------------------------------------------------------------------
CrateModel::CrateModel()
{
    this->init_time = std::time(nullptr);
    this->finish_time = -1;
}

void CrateModel::loadCrate(cv::Mat srcImg_, cv::Rect reg_, cv::Mat templateImg_)
{
    this->templateImage = templateImg_;
    this->crateImage = srcImg_(reg_);
}

void CrateModel::updateCrateImg(cv::Mat img_)
{

    // std::cout << "updating" << std::endl;
    this->crateImage = img_;
}

void checkIfImageMatch(){

}

void CrateModel::calcHist()
{
    
    
}

cv::Mat CrateModel::getCrateImage(){
    return this->crateImage;
}

cv::Mat CrateModel::getTemplateImage(){
    return this->templateImage;
}

void CrateModel::setCrateImage(cv::Mat crateImage_){
    this->crateImage =  crateImage_;
}

void CrateModel::setTemplateImage(cv::Mat templImage_){
    this->templateImage = templImage_;
}
// ----------------------------------------------------------------------------
// -- CRATE CLASS METHODS DEFINITION ------------------------------------------
// ----------------------------------------------------------------------------

Crate::Crate(cv::Mat srcImg_, cv::Rect reg_, cv::Mat prodTmplt)
{
    this->currentModel.loadCrate(srcImg_, reg_, prodTmplt);
    this->rightModel.loadCrate(srcImg_, reg_, prodTmplt);
    this->regionInFloor = reg_;
    this->productIsRight = true;
    this->isEmpty = false;  
}

void Crate::updateCrate(cv::Mat floorImg_)
{
    updateCrateImg(floorImg_);
    checkModels();
}

void Crate::checkModels(){
    this->calcIfEmpty();

    this->productIsRight = stky::doTheyMatch(rightModel.getTemplateImage(),
                                             currentModel.getCrateImage(),
                                             6, 0.25);
    if(this->productIsRight){
        currentModel.setTemplateImage(rightModel.getTemplateImage());
        // std::cout << "Producto Correcto" << std::endl;
        isEmpty = false;
    }
    else{
        // std::cout << "Producto Incorrecto" << std::endl;
    }
    
}

void Crate::updateCrateImg(cv::Mat floorImg_){
    cv::Mat crateImg = floorImg_(this->regionInFloor);
    this->currentModel.updateCrateImg(crateImg);
}

cv::Rect Crate::getRect(){
    return this->regionInFloor;
}

cv::Mat Crate::getCurrentImage()
{
    return this->currentModel.getCrateImage();
}

void Crate::calcIfEmpty(){
    cv::Mat crateImg = this->currentModel.getCrateImage();
    // cv::imshow("crate", crateImg);
    cv::Mat thresh;
    cv::cvtColor(crateImg, thresh, cv::COLOR_BGR2GRAY);
    
    cv::threshold(thresh, thresh, 120, 255, 0);
    // cv::imshow("Crate threshold", thresh);
    // cv::waitKey(0);

    cv::medianBlur(thresh, thresh, 11);
    cv::Size morph_size(thresh.cols*0.1, thresh.cols*0.1);
    cv::Mat morph_struct = getStructuringElement(cv::MORPH_RECT, morph_size);
    cv::dilate(thresh, thresh, morph_struct, cv::Point(-1, -1));
    cv::erode(thresh, thresh, morph_struct, cv::Point(-1, -1));

    // cv::imshow("Crate threshold", thresh);
    // cv::waitKey(0);
    
    cv::Mat M = thresh;
    int i, j, cols = M.cols,
    rows = M.rows;
    int depth = M.depth(),
    channels = M.channels(),
    step = M.step;
    uchar *data = M.data;
    int whitePix = 0;
    for(i = 0; i < rows; i++) {
        for(j = 0; j < cols; j++) {
            if( data[i*step + j*channels + 0] >= 128)
                whitePix++;
        }
    }
    if (whitePix < rows*cols*0.1)
        this->isEmpty = true;
    else
        this->isEmpty = false;

    // std::string message = isEmpty?"true":"false";
    // std::cout << message << std::endl;
}

bool Crate::isItEmpty(){
    return this->isEmpty;
}

bool Crate::isItRight(){
    return this->productIsRight;
}