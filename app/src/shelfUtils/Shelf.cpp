#include "shelfUtils/shelfUtils.hpp"

// ----------------------------------------------------------------------------
// -- SHELF CLASS METHODS DEFINITION ------------------------------------------
// ----------------------------------------------------------------------------

Shelf::Shelf(cv::Mat shelfImage_, int emptyThreshold,
             std::string const jsonPath, bool chargeFromJson, int id)
{
    this->shelfImage = shelfImage_.clone();
    if (jsonPath.compare("")!=0 ) // path no nulo
    {
        fgDidMove = false;
        last_movement_time = -1;
        this->dataPath = jsonPath;

        this->emptyThreshold = emptyThreshold;
        loadProductsFromJson(jsonPath, id);
        if (chargeFromJson)
            Shelf::loadShelfFromJson(jsonPath, id);
        else
            this->calcShelfInfo(shelfImage_);
        
        this->calcShelfMask(shelfImage_.rows, shelfImage_.cols);
        this->fillFloorsVect(shelfImage_.rows, shelfImage_.cols);
        this->calcEmptyMask();
        //this->paintFloorRects(shelfImage);

    }
    else
    {
        std::cerr << "Invalid path" << std::endl;
    }
}

void Shelf::loadShelfFromJson(std::string const jsonPath, int id)
{
    // cv::FileStorage json = cv::FileStorage("app/data/data.json", cv::FileStorage::READ);
    cv::FileStorage json;
    try{
        json.open(jsonPath, cv::FileStorage::READ);
    }catch(cv::Exception ex)
    {
        return;
    }
    cv::FileNode shelves = json["shelves"];
    for(int i = 0; i<shelves.size(); i++){
        if(shelves[i]["id"].string().compare(std::to_string(id)) == 0){
            cv::FileNode shelfNode = shelves[i];
            std::string thick_str = shelfNode["thickness"].string();
            thickness = stoi( thick_str );

            for(int j = 0; j< shelfNode["centers"].size(); j++){
                std::string center_str = shelfNode["centers"][j].string();
                centers.push_back(stoi(center_str));
            }

            for(int j = 0; j< shelfNode["products"].size(); j++){
                std::string product_str = shelfNode["products"][i].string();
                productsList.push_back(product_str);
            }
            break;
        }
    }
}

void Shelf::loadProductsFromJson(std::string const jsonPath, int id){
    // cv::FileStorage json = cv::FileStorage("app/data/data.json", cv::FileStorage::READ);
    cv::FileStorage json;
    try{
        json.open("app/data/data.json", cv::FileStorage::READ);
    }catch(cv::Exception ex)
    {
        return;
    }
    cv::FileNode products = json["products"];
    for (int i=0; i<products.size(); i++){
        cv::FileNode tmp = products[i];
        std::string tmpName = tmp["name"].string();
        for (int j=0; j<productsList.size(); j++){
            if ( tmpName.compare(productsList.at(j))==0 ){
                productsNodes.push_back(tmp);
            }
        }
    }
}

void Shelf::updateShelf(cv::Mat frame_, cv::Mat fg_){
    updateImage(frame_);
    setFgMask(fg_);
    if(this->isMoving()){
        fgDidMove  = true;
        this->last_movement_time = std::time(nullptr);
    }

    if(!this->isMoving() && this->fgDidMove){
        fgDidMove  = false;
        for(int i = 0; i<floors.size(); i++){
            // std::cout << "Piso " << i << std::endl;
            floors.at(i).reactToMovementEnd();
        }
    } 

}

void Shelf::updateImage(cv::Mat frame)
{
    this->shelfImage = frame.clone();
    // cv::imshow("video", this->shelfImage);

    for(int i=0; i<this->floors.size(); i++){
        floors.at(i).updateImage(this->shelfImage);
    }

    // cv::imshow("piso", this->floors.at(1).getFloorImage());

}

// void Shelf::updateFgMask(cv::Mat fg_)
// {
//     this->foregroundMask = fg_;

//     int diameter = (fg_.cols + fg_.rows)/ (2 * 20); 
//     cv::Size size(diameter, diameter);
//     cv::Mat structEl = getStructuringElement(cv::MORPH_ELLIPSE, size);
//     cv::erode(this->foregroundMask, this->foregroundMask, structEl, cv::Point(-1, -1));
//     cv::dilate(this->foregroundMask, this->foregroundMask, structEl, cv::Point(-1, -1));

//     if(~floors.empty()){
//         for(int i=0; i < floors.size(); i++)
//         {
//             floors.at(i).setFgMask(this->foregroundMask);
//         }
//     }
// }

void Shelf::checkIfFgIntersectsFloors()
{
    int diameter = (this->foregroundMask.cols)/(10); 
    cv::Size size(diameter, diameter);
    cv::Mat structEl = getStructuringElement(cv::MORPH_ELLIPSE, size);
    cv::Mat dilatedFg;
    cv::dilate(this->foregroundMask, dilatedFg, structEl, cv::Point(-1, -1));
    
    for(int k=0; k < this->floors.size(); k++){
        cv::Rect floorRect = floors.at(k).getFloorRect();
        cv::Mat floorMaskImg = dilatedFg(floorRect);
        uchar* data = floorMaskImg.data;
        int maskPix = 0;
        // cv::imshow("floor", floorMaskImg);
        // cv::waitKey(0);
        for(int i = 0; i < floorMaskImg.rows; i++) {
            for(int j = 0; j < floorMaskImg.cols; j++) {
            if (data[i*floorMaskImg.step + j*floorMaskImg.channels() + 0] > 128)
                maskPix++;
            }
            if (maskPix > floorMaskImg.rows*floorMaskImg.cols*0.1){
                floors.at(k).checkIfFgIntersectsCrates();
                break;
            }
            
        }

    }

}

void Shelf::calcShelfInfo(cv::Mat image)
{
    int ksize = 1;
    int scale = 2;
    int delta = 0;
    int ddepth = CV_16S;

    //imshow("Original", image);

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
    cv::Size hor_dilate_size(grad_y.cols * 2, grad_y.rows / 100);

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

    for (int i = 0; i < grad_y.rows; i++)
    {
        if (grad_y.at<unsigned int>(i, 0) > 128)
        {
            puntos_linea.push_back((int)i);
            last_was_white = true;
        }
        else
        {
            if (last_was_white)
            {
                this->centers.push_back(avrg_row(puntos_linea));
                puntos_linea.clear();
                line_counter++;
            }
            last_was_white = false;
        }
    }
    for (int i = 0; i < this->centers.size() - 1; i++)
    {
        int current = this->centers.at(i);
        int next = this->centers.at(i + 1);

        if ((next - current) < 3 * this->thickness)
        {
            this->centers[i] = (next + current) / 2;
            this->centers.erase(this->centers.begin() + i + 1);
        }
        else
        {
            this->centers[i] = current - 0.6 * this->thickness;
            if (this->centers[i] < 0)
                this->centers[i] = 0;
        }
    }
}

cv::Mat Shelf::getShelfMask(int height, int width)
{
    calcShelfMask(height, width);
    return this->shelfMask;
}

void Shelf::fillFloorsVect(int height, int width)
{
    int thick = this->thickness;
    for (int i = 0; i < this->centers.size() - 1; i++)
    {
        if (i == 0)
        { // Primer Rect (si hay suficiente espacio)
            if (this->centers.at(i) > 2 * thick)
            {
                cv::Rect temp_rect(0, thick / 2, width, this->centers.at(i) - thick);
                Floor temp_floor(shelfImage, temp_rect, this->foregroundMask,
                                 dataPath, productsNodes, productsList);
                this->floors.push_back(temp_floor);
            }
        }
        // Rect entre medio
        cv::Rect temp_rect(0, this->centers.at(i) + thick / 2, width,
                           this->centers.at(i + 1) - this->centers.at(i) - thick);
        Floor temp_floor(shelfImage, temp_rect, this->foregroundMask, 
                         dataPath, productsNodes, productsList);
        this->floors.push_back(temp_floor);
    }

    int last_index = this->centers.size() - 1;

    // Último rect (si hay espacio)
    if ((height - this->centers.at(last_index)) > 4 * this->thickness)
    {
        int a = height - this->centers.at(last_index) - 1;
        cv::Rect temp_rect(0, this->centers.at(last_index) + thick / 2, width,
                           height - this->centers.at(last_index) - thick / 2 - 1);
        Floor temp_floor(shelfImage, temp_rect, this->foregroundMask,
                         dataPath, productsNodes, productsList);
        this->floors.push_back(temp_floor);
    }
}

void Shelf::calcEmptyMask() 
{
    if( ~this->emptyMask.empty())
        this->emptyMask = cv::Mat::zeros(this->shelfImage.size(), CV_8UC1);
    for (int i=0; i < (this->floors.size()); i++)
    {   
        this->floors.at(i).calcEmptyMask(shelfImage, this->emptyThreshold);
        cv::Rect floorRect = this->floors.at(i).getFloorRect();
        this->floors.at(i).getEmptyMask().copyTo(this->emptyMask(floorRect));
        
    }
    // cv::imshow("Máscara Espacios Vacío", this->emptyMask);
    
    // cv::waitKey(0);
}

void Shelf::calcWrongProductsMask()
{
    this->wrongProductMask = cv::Mat(this->shelfImage.rows,
                                     this->shelfImage.cols,
                                     CV_8UC1, cv::Scalar(0, 0, 0));
    for(int i=0; i< floors.size(); i++){
        this->floors.at(i).calcWrongProductMask();
        cv::Rect floorRect = floors.at(i).getFloorRect();
        cv::Mat floorWrongMask = floors.at(i).getWrongProductsMask();
        floorWrongMask.copyTo( this->wrongProductMask(floorRect));
        // cv::imshow("floor wrong", floorWrongMask);
        // cv::waitKey(0);
    }
}

void Shelf::calcShelfMask(int height, int width)
{
    cv::Mat mask(height, width, CV_8UC3, cv::Scalar(0, 0, 0));

    for (int i = 0; i < this->centers.size(); i++)
    {
        cv::line(mask,
             cv::Point(0, this->centers.at(i)),
             cv::Point(width - 1, this->centers.at(i)),
             cv::Scalar(255, 255, 255), this->thickness,
             cv::LINE_8, 0);
    }

    //cv::imshow("Mascara", mask);
    this->shelfMask = mask;
}

int Shelf::avrg_row(std::vector<int> rows)
{
    int avrg = 0;
    int len = rows.size();
    for (int i = 0; i < len; i++)
    {
        int val = rows.back();
        avrg += rows.back();
        rows.pop_back();
    }
    avrg /= len;
    return avrg;
}

int Shelf::get_lines_thickness(cv::Mat img)
{
    int line_counter = 0;
    bool last_was_white = false;
    for (int i = 0; i < img.rows; i++)
    {
        if (img.at<unsigned int>(i, 0) > 128)
        {

            last_was_white = true;
        }
        else
        {
            if (last_was_white)
            {
                line_counter++;
            }
            last_was_white = false;
        }
    }
    int thickness = 0.3 * (img.rows / line_counter);
    return thickness;
}

void Shelf::paintFloorRects(cv::Mat image)
{
    int height = image.rows;
    int width = image.cols;

    cv::Mat floorRectsImage = image.clone();

    cv::Scalar colores[] = {
        cv::Scalar(0, 0, 255),
        cv::Scalar(0, 255, 255),
        cv::Scalar(255, 0, 255),
        cv::Scalar(0, 255, 0),
        cv::Scalar(255, 255, 0),
        cv::Scalar(255, 0, 0)};

    double alpha = 0.5;

    for (int i = 0; i < (this->floors.size()); i++)
    {
        Floor temp_floor = this->floors.at(i);
        cv::Rect roi = temp_floor.getFloorRect();
        cv::Mat roiMat = floorRectsImage(roi);
        cv::Mat color(roi.size(), CV_8UC3, colores[i]);
        cv::addWeighted(color, alpha, roiMat, 1.0 - alpha, 0.0, roiMat);
    }

    cv::imshow("Alpha", floorRectsImage);
}

Floor Shelf::getFloor(int floor)
{
    return this->floors.at(floor);
}

cv::Mat Shelf::getShelfImage(){
    return this->shelfImage;
}

cv::Mat Shelf::getEmptyMask(){
    // this->calcEmptyMask();
    return this->emptyMask;
}

void Shelf::setFgMask(cv::Mat fgMask_){
    this->foregroundMask = fgMask_;
    for(int i = 0; i < floors.size(); i++){
        floors.at(i).setFgMask(fgMask_);
    }
}

cv::Mat Shelf::getFgMask(){
    return this->foregroundMask;
}

cv::Mat Shelf::getWrongProductMask(){
    return this->wrongProductMask;
}

bool Shelf::isMoving(){
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

void Shelf::drawGuiImage(bool drawEmpty, bool drawWrong){
    this->guiImage = this->shelfImage.clone();
    if (drawEmpty)
        stky::colorBlobsInImage(this->guiImage, 
                                this->emptyMask, 
                                cv::Scalar(255,0,0));
    if (drawWrong)
        stky::colorBlobsInImage(this->guiImage, 
                                this->wrongProductMask, 
                                cv::Scalar(0,0,255));
}

cv::Mat Shelf::getGuiImage(){
    return this->guiImage;
}