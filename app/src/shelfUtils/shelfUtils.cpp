#include <shelfUtils/shelfUtils.hpp>

// ----------------------------------------------------------------------------
// -- CRATE MODEL CLASS METHODS DEFINITION ------------------------------------
// ----------------------------------------------------------------------------
// CrateModel::CrateModel(cv::Mat srcImg_, cv::Rect reg_, cv::Mat templateImg_)
// {
//     this->templateImage = templateImg_;
//     this->crateImage = srcImg_(reg_);
//     this->init_time = std::time(nullptr);
//     this->finish_time = -1;
    
// }

// void CrateModel::calcHist()
// {
    
    
// }


// ----------------------------------------------------------------------------
// -- CRATE CLASS METHODS DEFINITION ------------------------------------------
// ----------------------------------------------------------------------------

Crate::Crate(cv::Mat srcImg_, cv::Rect reg_)
{
    //Pasarle imagen a Right Model
}

// ----------------------------------------------------------------------------
// -- FLOOR CLASS METHODS DEFINITION ------------------------------------------
// ----------------------------------------------------------------------------

Floor::Floor(cv::Mat shelfImage, cv::Rect rect)
{
    this->floorRect = rect;
    this->floorImage = shelfImage(rect);
}

void Floor::updateImage(cv::Mat shelfImage){
    this->floorImage = shelfImage(this->floorRect);
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
    for(int i=0; i<products_.size(); i++){
        products.push_back(products_.at(i));
    }
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

    cv::Size dilate_size(thresh.cols / 50, thresh.rows * 2);
    cv::Mat dilate_struct = getStructuringElement(cv::MORPH_RECT, dilate_size);
    cv::dilate(thresh, mask, dilate_struct, cv::Point(-1, -1));

    cv::Size size2(thresh.cols / 50, thresh.cols / 50);
    cv::Mat struct2 = getStructuringElement(cv::MORPH_RECT, size2);
    cv::erode(mask, mask, struct2, cv::Point(-1, -1), 1, cv::BORDER_ISOLATED);

    cv::bitwise_not(mask, mask);

    // cv::imshow("FloorMask", mask);

    this->emptyMask = mask;
}

void Floor::calcCrates()
{
    std::vector<cv::Rect> boxes;
    stky::scanFeaturesSlidingWindow(tmpltImg, floorImage, boxes);
}
// ----------------------------------------------------------------------------
// -- SHELF CLASS METHODS DEFINITION ------------------------------------------
// ----------------------------------------------------------------------------

Shelf::Shelf(cv::Mat shelfImage, int emptyThreshold, std::string jsonPath, int id)
{
    if (jsonPath.compare("")!=0)
    {
        this->shelfImage = shelfImage.clone();
        this->calcShelfInfo(shelfImage);
        this->getShelfMask(shelfImage.rows, shelfImage.cols);
        this->fillFloorsVect(shelfImage.rows, shelfImage.cols);
        //this->paintFloorRects(shelfImage);
        this->emptyThreshold = emptyThreshold;
    }
    else
    {
        Shelf::loadJSON(jsonPath, id);
    }
    
}

// Shelf::Shelf(std::string jsonPath, int id)
// {
//     Shelf::loadJSON(jsonPath, id);
// }

void Shelf::loadJSON(std::string jsonPath, int id)
{
    cv::FileStorage json("app/data/data.json", cv::FileStorage::READ);
    cv::FileNode shelves = json["shelves"];
    for(int i = 0; i<shelves.size(); i++){
        if(shelves[i]["id"].string().compare(std::to_string(id)) == 0){
            cv::FileNode shelf = shelves[i];
            std::string thick_str = shelf["thickness"].string();
            thickness = stoi( thick_str );

            for(int j = 0; j< shelf["centers"].size(); j++){
                std::string center_str = shelf["centers"][i].string();
                centers.push_back(stoi(center_str));
            }

            for(int j = 0; j< shelf["products"].size(); j++){
                std::string product_str = shelf["products"][i].string();
                products.push_back(product_str);
            }

            break;
        }
    }
    if(!shelfImage.empty()){
        fillFloorsVect(shelfImage.rows, shelfImage.cols);
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
                Floor temp_floor(shelfImage, temp_rect);
                this->floors.push_back(temp_floor);
            }
        }
        // Rect entre medio

        cv::Rect temp_rect(0, this->centers.at(i) + thick / 2, width,
                           this->centers.at(i + 1) - this->centers.at(i) - thick);
        Floor temp_floor(shelfImage, temp_rect);
        this->floors.push_back(temp_floor);
    }

    int last_index = this->centers.size() - 1;

    // Último rect (si hay espacio)
    if ((height - this->centers.at(last_index)) > 4 * this->thickness)
    {
        int a = height - this->centers.at(last_index) - 1;
        cv::Rect temp_rect(0, this->centers.at(last_index) + thick / 2, width,
                           height - this->centers.at(last_index) - thick / 2 - 1);
        Floor temp_floor(shelfImage, temp_rect);
        this->floors.push_back(temp_floor);
    }

    for( int i=0; i < floors.size(); i++){
        floors.at(i).setProducts(products);
    }

}

void Shelf::calcEmptyMask() 
{
    cv::Mat emptyMask(this->shelfImage.rows, this->shelfImage.cols, 
                      CV_8UC1, cv::Scalar(0, 0, 0));
    this->emptyMask = emptyMask.clone(); // Se puede mejorar un poco aca

    for (int i; i < (this->floors.size()); i++)
    {
        this->floors.at(i).calcEmptyMask(shelfImage, this->emptyThreshold);
        cv::Rect floorRect = this->floors.at(i).getFloorRect();
        cv::Mat floorMask = this->floors.at(i).getEmptyMask();
        floorMask.copyTo(this->emptyMask(floorRect));
    }
    // cv::imshow("Máscara Espacios Vacío", this->emptyMask);
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

    // cv::imshow("Mascara", mask);
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