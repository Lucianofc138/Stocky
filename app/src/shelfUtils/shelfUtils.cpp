// #include <shelfUtils/shelfUtils.hpp>

// // ----------------------------------------------------------------------------
// // -- CRATE MODEL CLASS METHODS DEFINITION ------------------------------------
// // ----------------------------------------------------------------------------
// CrateModel::CrateModel()
// {
//     this->init_time = std::time(nullptr);
//     this->finish_time = -1;
// }

// // CrateModel::CrateModel(cv::Mat srcImg_, cv::Rect reg_, cv::Mat templateImg_)
// // {
// //     this->templateImage = templateImg_;
// //     this->crateImage = srcImg_(reg_);
// //     this->init_time = std::time(nullptr);
// //     this->finish_time = -1;
// // }
// void CrateModel::loadCrate(cv::Mat srcImg_, cv::Rect reg_, cv::Mat templateImg_)
// {
//     this->templateImage = templateImg_;
//     this->crateImage = srcImg_(reg_);

// }
// void CrateModel::updateCrateImg(cv::Mat img_)
// {
//     this->crateImage = img_;
// }

// void CrateModel::calcHist()
// {
    
    
// }


// // ----------------------------------------------------------------------------
// // -- CRATE CLASS METHODS DEFINITION ------------------------------------------
// // ----------------------------------------------------------------------------

// Crate::Crate(cv::Mat srcImg_, cv::Rect reg_, cv::Mat prodTmplt)
// {
//     this->currentModel.loadCrate(srcImg_, reg_, prodTmplt);
//     this->rightModel.loadCrate(srcImg_, reg_, prodTmplt);
//     this->regionInFloor = reg_;
//     this->productIsRight = true;
//     this->isEmpty = false;  
// }

// void Crate::updateCrateImg(cv::Mat floorImg_){
//     cv::Mat crateImg = floorImg_(this->regionInFloor);
//     this->currentModel.updateCrateImg(crateImg);
//     if (this->productIsRight)
//         this->rightModel.updateCrateImg(crateImg);
// }

// cv::Rect Crate::getRect(){
//     return this->regionInFloor;
// }

// // ----------------------------------------------------------------------------
// // -- FLOOR CLASS METHODS DEFINITION ------------------------------------------
// // ----------------------------------------------------------------------------

// Floor::Floor(cv::Mat shelfImage, cv::Rect rect, cv::Mat fgMask_, std::string path_,  
//              std::vector< cv::FileNode > productsNodes_,
//              std::vector< std::string > productsList_)
// {
//     this->floorRect = rect;
//     this->floorImage = shelfImage(rect);
//     this->productsNodes = productsNodes_;
//     this->productsList = productsList_;
//     this->dataPath = path_;
//     this->foregroundMask = fgMask_;
//     //this->calcCrates();
// }

// void Floor::updateImage(cv::Mat shelfImage){
//     this->floorImage = shelfImage(this->floorRect);
// }   

// void Floor::setFgMask(cv::Mat fg_){
//     this->foregroundMask = fg_(this->floorRect);
// }

// void Floor::checkIfFgIntersectsCrates(){
//     int diameter = (this->foregroundMask.cols)/(10); 
//     cv::Size size(diameter, diameter);
//     cv::Mat structEl = getStructuringElement(cv::MORPH_ELLIPSE, size);
//     cv::Mat dilatedFg;
//     cv::dilate(this->foregroundMask, dilatedFg, structEl, cv::Point(-1, -1));

//     for(int k = 0; k < this->crates.size(); k++)
//     {
//         Crate currCrate = this->crates.at(k);
//         cv::Rect crateRect = currCrate.getRect();
//         cv::Mat crateDilMask = dilatedFg(floorRect);
//         uchar* data = crateDilMask.data;
//         int maskPix = 0;
//         for(int i = 0; i < crateDilMask.rows; i++) {
//             for(int j = 0; j < crateDilMask.cols; j++) {
//             if (data[i*crateDilMask.step + j*crateDilMask.channels() + 0] > 128)
//                 maskPix++;
//             }
//             if (maskPix > crateDilMask.rows*crateDilMask.cols*0.05){
//                 currCrate.updateCrateImg(this->floorImage(crateRect));
//                 break;
//             }
//         }
//     }
// }

// cv::Rect Floor::getFloorRect()
// {
//     return this->floorRect;
// }

// cv::Mat Floor::getFloorImage()
// {
//     return this->floorImage;
// }

// cv::Mat Floor::getEmptyMask()
// {
//     return this->emptyMask;
// }

// void Floor::setProducts( std::vector< std::string > products_ ){
//     // for(int i=0; i<products_.size(); i++){
//     //     products.push_back(products_.at(i));
//     // }
// }

// void Floor::calcEmptyMask(cv::Mat src_img, int emptyThreshold)
// {
//     // cv::Size floorSize = this->floorRect.size();
//     // cv::Mat src(floorSize, CV_8UC4);
//     cv::Mat src_rect = src_img(this->floorRect);
//     cv::Mat gray, thresh, mask;
//     cv::cvtColor(src_rect, gray, cv::COLOR_BGR2GRAY);
//     cv::equalizeHist(gray, gray);
//     cv::threshold(gray, thresh, emptyThreshold, 255, 0);

//     // cv::imshow("FloorThresh", thresh);

//     cv::Size erode_size1(thresh.cols / 200, thresh.rows / 15 );
//     cv::Mat erode_struct1 = getStructuringElement(cv::MORPH_RECT, erode_size1);
//     cv::erode(thresh, mask, erode_struct1, cv::Point(-1, -1));

//     // cv::imshow("mask1", mask);

//     cv::Size dilate_size1(thresh.cols / 40, thresh.rows * 2);
//     cv::Mat dilate_struct1 = getStructuringElement(cv::MORPH_RECT, dilate_size1);
//     cv::dilate(mask, mask, dilate_struct1, cv::Point(-1, -1));

//     // cv::imshow("mask2", mask);

//     cv::Size size2(thresh.cols / 25, 1);
//     cv::Mat dilate_struct2 = getStructuringElement(cv::MORPH_RECT, size2);
//     cv::dilate(mask, mask, dilate_struct2, cv::Point(-1, -1));

//     // cv::imshow("mask3", mask);
    
//     cv::Mat erode_struct2 = getStructuringElement(cv::MORPH_RECT, size2);
//     cv::erode(mask, mask, erode_struct2, cv::Point(-1, -1), 1, cv::BORDER_ISOLATED);

//     // cv::imshow("mask4", mask);

//     cv::bitwise_not(mask, mask);

//     // cv::imshow("FloorMask", mask);
//     // cv::waitKey(0);

//     this->emptyMask = mask;
// }

// void Floor::calcCrates()
// {
//     cv::FileStorage json = cv::FileStorage(this->dataPath, cv::FileStorage::READ);    
//     cv::FileNode allProducts = json["products"];

//     for (int i = 0; i<allProducts.size(); i++){
//         cv::FileNode currentProduct = allProducts[i];
//         for(int j=0; j<productsList.size(); j++){
//             std::string name1 = currentProduct["name"].string();
//             std::string nameInList = productsList.at(j);
//             if ( name1.compare(nameInList) == 0){
//                 std::string commonPath = currentProduct["path"].string();
//                 std::string name = currentProduct["name"].string();
//                 for(int k=0; k< 24; k++){
//                     std::string num = std::to_string(k);
//                     std::string path = commonPath + name + num +".jpg";
//                     cv::Mat templtImg = cv::imread(path);
//                     std::vector<cv::Rect> boxes;
//                     cv::imshow("template", templtImg);
//                     cv::imshow("floor", floorImage);
//                     cv::waitKey(0);
//                     stky::scanFeaturesSlidingWindow(templtImg, floorImage, boxes);
//                     for (int l=0; l<boxes.size(); l++){
//                         Crate tmpCrate(this->floorImage, boxes.at(l), templtImg);
//                         this->crates.push_back(tmpCrate);
//                     }
//                     // cv::waitKey(0);
//                     if (boxes.size() > 0)
//                         boxes.clear();
//                 }
//                 break;
//             }
//         }
//     }
// }

// void Floor::yaBastaFreezer(){
//     std::cout << "Detengan el semestre que me quiero bajar :'(" << std::endl;
// }
// // ----------------------------------------------------------------------------
// // -- SHELF CLASS METHODS DEFINITION ------------------------------------------
// // ----------------------------------------------------------------------------

// Shelf::Shelf(cv::Mat shelfImage_, int emptyThreshold,
//              std::string const jsonPath, bool chargeFromJson, int id)
// {
//     this->shelfImage = shelfImage_.clone();
//     if (jsonPath.compare("")!=0 ) // path no nulo
//     {
//         this->dataPath = jsonPath;
//         this->emptyThreshold = emptyThreshold;
//         loadProductsFromJson(jsonPath, id);
//         if (chargeFromJson)
//             Shelf::loadShelfFromJson(jsonPath, id);
//         else
//             this->calcShelfInfo(shelfImage_);
        
//         this->calcShelfMask(shelfImage_.rows, shelfImage_.cols);
//         this->fillFloorsVect(shelfImage_.rows, shelfImage_.cols);
//         this->calcEmptyMask();
//         //this->paintFloorRects(shelfImage);

//     }
//     else
//     {
//         std::cerr << "Invalid path" << std::endl;
//     }
// }

// void Shelf::loadShelfFromJson(std::string const jsonPath, int id)
// {
//     // cv::FileStorage json = cv::FileStorage("app/data/data.json", cv::FileStorage::READ);
//     cv::FileStorage json;
//     try{
//         json.open("app/data/data.json", cv::FileStorage::READ);
//     }catch(cv::Exception ex)
//     {
//         return;
//     }
//     cv::FileNode shelves = json["shelves"];
//     for(int i = 0; i<shelves.size(); i++){
//         if(shelves[i]["id"].string().compare(std::to_string(id)) == 0){
//             cv::FileNode shelfNode = shelves[i];
//             std::string thick_str = shelfNode["thickness"].string();
//             thickness = stoi( thick_str );

//             for(int j = 0; j< shelfNode["centers"].size(); j++){
//                 std::string center_str = shelfNode["centers"][j].string();
//                 centers.push_back(stoi(center_str));
//             }

//             for(int j = 0; j< shelfNode["products"].size(); j++){
//                 std::string product_str = shelfNode["products"][i].string();
//                 productsList.push_back(product_str);
//             }
//             break;
//         }
//     }
// }

// void Shelf::loadProductsFromJson(std::string const jsonPath, int id){
//     // cv::FileStorage json = cv::FileStorage("app/data/data.json", cv::FileStorage::READ);
//     cv::FileStorage json;
//     try{
//         json.open("app/data/data.json", cv::FileStorage::READ);
//     }catch(cv::Exception ex)
//     {
//         return;
//     }
//     cv::FileNode products = json["products"];
//     for (int i=0; i<products.size(); i++){
//         cv::FileNode tmp = products[i];
//         std::string tmpName = tmp["name"].string();
//         for (int j=0; j<productsList.size(); j++){
//             if ( tmpName.compare(productsList.at(j))==0 ){
//                 productsNodes.push_back(tmp);
//             }
//         }
//     }
// }

// void Shelf::updateImage(cv::Mat frame)
// {
//     this->shelfImage = frame.clone();
//     // cv::imshow("video", this->shelfImage);

//     for(int i=0; i<this->floors.size(); i++){
//         floors.at(i).updateImage(this->shelfImage);
//     }

//     // cv::imshow("piso", this->floors.at(1).getFloorImage());

// }

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

// void Shelf::checkIfFgIntersectsFloors()
// {
//     int diameter = (this->foregroundMask.cols)/(10); 
//     cv::Size size(diameter, diameter);
//     cv::Mat structEl = getStructuringElement(cv::MORPH_ELLIPSE, size);
//     cv::Mat dilatedFg;
//     cv::dilate(this->foregroundMask, dilatedFg, structEl, cv::Point(-1, -1));
    
//     for(int k=0; k < this->floors.size(); k++){
//         cv::Rect floorRect = floors.at(k).getFloorRect();
//         cv::Mat floorMaskImg = dilatedFg(floorRect);
//         uchar* data = floorMaskImg.data;
//         int maskPix = 0;
//         // cv::imshow("floor", floorMaskImg);
//         // cv::waitKey(0);
//         for(int i = 0; i < floorMaskImg.rows; i++) {
//             for(int j = 0; j < floorMaskImg.cols; j++) {
//             if (data[i*floorMaskImg.step + j*floorMaskImg.channels() + 0] > 128)
//                 maskPix++;
//             }
//             if (maskPix > floorMaskImg.rows*floorMaskImg.cols*0.1){
//                 floors.at(k).checkIfFgIntersectsCrates();
//                 break;
//             }
            
//         }

//     }

// }

// void Shelf::calcShelfInfo(cv::Mat image)
// {
//     int ksize = 1;
//     int scale = 2;
//     int delta = 0;
//     int ddepth = CV_16S;

//     //imshow("Original", image);

//     cv::Mat src, src_gray, grad_y;
//     // Remove noise by blurring with a Gaussian filter ( kernel size = 3 )
//     GaussianBlur(image, src, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
//     // src = image;
//     // Convert the image to grayscale
//     cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
//     equalizeHist(src_gray, src_gray);
//     // imshow("EqGray", src_gray);
//     Sobel(src_gray, grad_y, ddepth, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT);
//     convertScaleAbs(grad_y, grad_y);
//     equalizeHist(grad_y, grad_y);
//     // imshow("EqY", grad_y);

//     // Structuring elements and their size
//     cv::Size hor_erode_size(grad_y.cols / 4, 1);
//     cv::Size hor_dilate_size(grad_y.cols * 2, grad_y.rows / 100);

//     cv::Mat hor_erode_structure = getStructuringElement(cv::MORPH_RECT, hor_erode_size);
//     cv::Mat hor_dilate_structure = getStructuringElement(cv::MORPH_RECT, hor_dilate_size);

//     // Apply morphology operations
//     threshold(grad_y, grad_y, 224, 255, 0);
//     // imshow("Thresh", grad_y);
//     erode(grad_y, grad_y, hor_erode_structure, cv::Point(-1, -1));
//     // imshow("After Erode", grad_y);
//     dilate(grad_y, grad_y, hor_dilate_structure, cv::Point(-1, -1));

//     // imshow("First Mask", grad_y);

//     std::vector<int> puntos_linea;
//     int count = 0;
//     this->thickness = get_lines_thickness(grad_y);
//     int line_counter = 0;
//     bool last_was_white = false;

//     for (int i = 0; i < grad_y.rows; i++)
//     {
//         if (grad_y.at<unsigned int>(i, 0) > 128)
//         {
//             puntos_linea.push_back((int)i);
//             last_was_white = true;
//         }
//         else
//         {
//             if (last_was_white)
//             {
//                 this->centers.push_back(avrg_row(puntos_linea));
//                 puntos_linea.clear();
//                 line_counter++;
//             }
//             last_was_white = false;
//         }
//     }
//     for (int i = 0; i < this->centers.size() - 1; i++)
//     {
//         int current = this->centers.at(i);
//         int next = this->centers.at(i + 1);

//         if ((next - current) < 3 * this->thickness)
//         {
//             this->centers[i] = (next + current) / 2;
//             this->centers.erase(this->centers.begin() + i + 1);
//         }
//         else
//         {
//             this->centers[i] = current - 0.6 * this->thickness;
//             if (this->centers[i] < 0)
//                 this->centers[i] = 0;
//         }
//     }
// }

// cv::Mat Shelf::getShelfMask(int height, int width)
// {
//     calcShelfMask(height, width);
//     return this->shelfMask;
// }

// void Shelf::fillFloorsVect(int height, int width)
// {
//     int thick = this->thickness;
//     for (int i = 0; i < this->centers.size() - 1; i++)
//     {
//         if (i == 0)
//         { // Primer Rect (si hay suficiente espacio)
//             if (this->centers.at(i) > 2 * thick)
//             {
//                 cv::Rect temp_rect(0, thick / 2, width, this->centers.at(i) - thick);
//                 Floor temp_floor(shelfImage, temp_rect, this->foregroundMask,
//                                  dataPath, productsNodes, productsList);
//                 this->floors.push_back(temp_floor);
//             }
//         }
//         // Rect entre medio

//         cv::Rect temp_rect(0, this->centers.at(i) + thick / 2, width,
//                            this->centers.at(i + 1) - this->centers.at(i) - thick);
//         Floor temp_floor(shelfImage, temp_rect, this->foregroundMask, 
//                          dataPath, productsNodes, productsList);
//         this->floors.push_back(temp_floor);
//     }

//     int last_index = this->centers.size() - 1;

//     // Último rect (si hay espacio)
//     if ((height - this->centers.at(last_index)) > 4 * this->thickness)
//     {
//         int a = height - this->centers.at(last_index) - 1;
//         cv::Rect temp_rect(0, this->centers.at(last_index) + thick / 2, width,
//                            height - this->centers.at(last_index) - thick / 2 - 1);
//         Floor temp_floor(shelfImage, temp_rect, this->foregroundMask,
//                          dataPath, productsNodes, productsList);
//         this->floors.push_back(temp_floor);
//     }
// }

// void Shelf::calcEmptyMask() 
// {
//     cv::Mat emptyMask(this->shelfImage.rows, this->shelfImage.cols, 
//                       CV_8UC1, cv::Scalar(0, 0, 0));
//     this->emptyMask = emptyMask.clone(); // Se puede mejorar un poco aca

//     for (int i=0; i < (this->floors.size()); i++)
//     {
//         this->floors.at(i).calcEmptyMask(shelfImage, this->emptyThreshold);
//         cv::Rect floorRect = this->floors.at(i).getFloorRect();
//         cv::Mat floorMask = this->floors.at(i).getEmptyMask();
//         floorMask.copyTo(this->emptyMask(floorRect));
//     }
//     cv::imshow("Máscara Espacios Vacío", this->emptyMask);
//     cv::waitKey(0);
// }

// void Shelf::calcShelfMask(int height, int width)
// {
//     cv::Mat mask(height, width, CV_8UC3, cv::Scalar(0, 0, 0));

//     for (int i = 0; i < this->centers.size(); i++)
//     {
//         cv::line(mask,
//              cv::Point(0, this->centers.at(i)),
//              cv::Point(width - 1, this->centers.at(i)),
//              cv::Scalar(255, 255, 255), this->thickness,
//              cv::LINE_8, 0);
//     }

//     //cv::imshow("Mascara", mask);
//     this->shelfMask = mask;
// }

// int Shelf::avrg_row(std::vector<int> rows)
// {
//     int avrg = 0;
//     int len = rows.size();
//     for (int i = 0; i < len; i++)
//     {
//         int val = rows.back();
//         avrg += rows.back();
//         rows.pop_back();
//     }
//     avrg /= len;
//     return avrg;
// }

// int Shelf::get_lines_thickness(cv::Mat img)
// {
//     int line_counter = 0;
//     bool last_was_white = false;
//     for (int i = 0; i < img.rows; i++)
//     {
//         if (img.at<unsigned int>(i, 0) > 128)
//         {

//             last_was_white = true;
//         }
//         else
//         {
//             if (last_was_white)
//             {
//                 line_counter++;
//             }
//             last_was_white = false;
//         }
//     }
//     int thickness = 0.3 * (img.rows / line_counter);
//     return thickness;
// }

// void Shelf::paintFloorRects(cv::Mat image)
// {
//     int height = image.rows;
//     int width = image.cols;

//     cv::Mat floorRectsImage = image.clone();

//     cv::Scalar colores[] = {
//         cv::Scalar(0, 0, 255),
//         cv::Scalar(0, 255, 255),
//         cv::Scalar(255, 0, 255),
//         cv::Scalar(0, 255, 0),
//         cv::Scalar(255, 255, 0),
//         cv::Scalar(255, 0, 0)};

//     double alpha = 0.5;

//     for (int i = 0; i < (this->floors.size()); i++)
//     {
//         Floor temp_floor = this->floors.at(i);
//         cv::Rect roi = temp_floor.getFloorRect();
//         cv::Mat roiMat = floorRectsImage(roi);
//         cv::Mat color(roi.size(), CV_8UC3, colores[i]);
//         cv::addWeighted(color, alpha, roiMat, 1.0 - alpha, 0.0, roiMat);
//     }

//     cv::imshow("Alpha", floorRectsImage);
// }

// Floor Shelf::getFloor(int floor)
// {
//     return this->floors.at(floor);
// }

