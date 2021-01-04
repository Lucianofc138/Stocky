// #include <iostream>
// #include <vector>    

// #include <opencv2/imgproc.hpp>
// #include <opencv2/highgui/highgui.hpp>


// int main( int argc, char** argv ) { 
// /* Load input image */
//     int a = 1;
//     cv::Mat img;
//     img = cv::imread(argv[1]);
//     if (img.empty())
//     {
//         std::cout << "!!! Failed to open image: " << argv[1]<< std::endl;
//         return -1;
//     }

//     /* Convert to grayscale */

//     cv::Mat gray;
//     cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

//     /* Histogram equalization improves the contrast between dark/bright areas */

//     cv::Mat equalized;
//     cv::equalizeHist(gray, equalized);
//     // cv::imwrite(std::string("eq_" + std::to_string(a) + ".jpg"), equalized);equalized, blur, 9
//     cv::imshow("Hist. Eq.", equalized);

//     /* Bilateral filter helps to improve the segmentation process */

//     cv::Mat blur;
//     cv::bilateralFilter(equalized, blur, 9, 75, 75);
//     // cv::medianBlur(equalized, blur, 3);
//     // cv::imwrite(std::string("filter_" + std::to_string(a) + ".jpg"), blur);
//     cv::imshow("Filter", blur);

//     /* Threshold to binarize the image */

//     cv::Mat thres;
//     cv::adaptiveThreshold(blur, thres, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 15, 2); //15, 2
//     // cv::imwrite(std::string("thres_" + std::to_string(a) + ".jpg"), thres);
//     cv::imshow("Threshold", thres);

//     // /* Remove small segments and the extremelly large ones as well */

//     // std::vector<std::vector<cv::Point> > contours;
//     // cv::findContours(thres, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

//     // double min_area = 50;
//     // double max_area = 2000;
//     // std::vector<std::vector<cv::Point> > good_contours;
//     // for (size_t i = 0; i < contours.size(); i++)
//     // {
//     //     double area = cv::contourArea(contours[i]);
//     //     if (area > min_area && area < max_area)
//     //         good_contours.push_back(contours[i]);
//     // }

//     // cv::Mat segments(gray.size(), CV_8U, cv::Scalar(255));
//     // cv::drawContours(segments, good_contours, -1, cv::Scalar(0), cv::FILLED, 4);
//     // // cv::imwrite(std::string("segments_" + std::to_string(a) + ".jpg"), segments);
//     // cv::imshow("Segments", segments);

//     cv::Mat final;
//     // cv::Mat repisa = cv::imread("repisa_mask_4.jpg");
//     // cv::cvtColor(repisa, repisa, cv::COLOR_BGR2GRAY);
//     // cv::threshold(repisa, repisa, 128, 255, 0);    

//     // cv::addWeighted(thres, 1.0, repisa, -1.0, 0, final, -1);

//     cv::Mat thresh2;
//     cv::cvtColor(img, thresh2, cv::COLOR_BGR2GRAY);
//     cv::threshold(thresh2, thresh2, 128, 255, 0);
//     // cv::imshow("Thresh", thresh2);

//     cv::bitwise_not(thresh2, thresh2);
//     cv::addWeighted(thres, 1.0, thresh2, -1.0, 0, final, -1);


    
//     cv::imshow("Final", final);

//     cv::waitKey(0);
//     return 0;
// }