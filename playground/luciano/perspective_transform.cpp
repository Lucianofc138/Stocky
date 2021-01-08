// #include <opencv2/opencv.hpp>
// #include "opencv2/xfeatures2d.hpp"
// #include "opencv2/features2d.hpp"

// using namespace std;
// using namespace cv;
// using namespace cv::xfeatures2d;

// int main(int argc, char **argv)
// {

//     Mat img_orig = imread("perspective_samp.jpg");

//     cv::imshow("Original", img_orig);

//     std::vector< cv::Point2f > mySrcPoints;
//     cv::Point2f p1(1, 55); 
//     cv::Point2f p2(727, 98);
//     cv::Point2f p3(75, 533);
//     cv::Point2f p4(724, 273);
//     mySrcPoints.push_back(p1);
//     mySrcPoints.push_back(p2);
//     mySrcPoints.push_back(p3);
//     mySrcPoints.push_back(p4);

//     cv::Size dsize(img_orig.cols*1.5, img_orig.rows);
//     cv::Mat dst(dsize, CV_8UC3);
//     std::vector< cv::Point2f > myDstPoints;
//     myDstPoints.push_back(Point2f(0, 0));
//     myDstPoints.push_back(Point2f(dst.cols, 0));
//     myDstPoints.push_back(Point2f(0, dst.rows));
//     myDstPoints.push_back(Point2f(dst.cols, dst.rows));

//     Mat perspectiveMat_= findHomography(mySrcPoints, myDstPoints);
//     perspectiveTransform(mySrcPoints, myDstPoints, perspectiveMat_);
//     cv::warpPerspective( img_orig, dst, perspectiveMat_, dst.size(), BORDER_CONSTANT);
    
//     cv::imshow("Perpectiva", dst);
//     cv::waitKey(0);

//     return 0;
// }
