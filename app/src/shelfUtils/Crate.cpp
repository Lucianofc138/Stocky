#include "shelfUtils/shelfUtils.hpp"

// ----------------------------------------------------------------------------
// -- CRATE MODEL CLASS METHODS DEFINITION ------------------------------------
// ----------------------------------------------------------------------------
CrateModel::CrateModel()
{
    this->init_time = std::time(nullptr);
    this->finish_time = -1;
}

// CrateModel::CrateModel(cv::Mat srcImg_, cv::Rect reg_, cv::Mat templateImg_)
// {
//     this->templateImage = templateImg_;
//     this->crateImage = srcImg_(reg_);
//     this->init_time = std::time(nullptr);
//     this->finish_time = -1;
// }
void CrateModel::loadCrate(cv::Mat srcImg_, cv::Rect reg_, cv::Mat templateImg_)
{
    this->templateImage = templateImg_;
    this->crateImage = srcImg_(reg_);

}
void CrateModel::updateCrateImg(cv::Mat img_)
{
    this->crateImage = img_;
}

void CrateModel::calcHist()
{
    
    
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

void Crate::updateCrateImg(cv::Mat floorImg_){
    cv::Mat crateImg = floorImg_(this->regionInFloor);
    this->currentModel.updateCrateImg(crateImg);
    if (this->productIsRight)
        this->rightModel.updateCrateImg(crateImg);
}

cv::Rect Crate::getRect(){
    return this->regionInFloor;
}