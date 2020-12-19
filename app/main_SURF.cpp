#include <iostream>
#include <algorithm>
#include "opencv2/core.hpp"
#include <opencv2/opencv.hpp>

#ifdef HAVE_OPENCV_XFEATURES2D
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
using namespace cv;
using namespace cv::xfeatures2d;
using std::cout;
using std::endl;

void sortKeypoints(std::vector<KeyPoint> &k_query,
                   std::vector<KeyPoint> &k_train, 
                   std::vector<DMatch> &good_matches,
                   std::vector<KeyPoint> &k_query2,
                   std::vector<KeyPoint> &k_train2 ){

    for(int i=0; i<good_matches.size(); i++){
        int min_x = k_train.at( good_matches.at(i).trainIdx ).pt.x;
        int min_pos = i;
        for(int j=i+1; j<good_matches.size(); j++){
            int current_x = k_train.at( good_matches.at(j).trainIdx ).pt.x;
            if( current_x < min_x ){
                min_pos = j;
                min_x = current_x;
            }
        }
        k_query2.push_back(k_query.at(good_matches[min_pos].queryIdx));
        k_train2.push_back(k_train.at(good_matches[min_pos].trainIdx));
        std::swap(good_matches[i], good_matches[min_pos]);
        good_matches[i].queryIdx = i;
        good_matches[i].trainIdx = i;
        

    }

}

void filterKeypoints(std::vector<KeyPoint> &k_query,
                          std::vector<KeyPoint> &k_train, 
                          std::vector<DMatch> &good_matches,
                          int max_dist, int min_dist){

    std::vector<int> pos_to_keep;

    for(int i = 1; i<k_train.size()-1; i++){
        int prev_x = k_train.at(i-1).pt.x;
        int current_x = k_train.at(i).pt.x;
        int next_x = k_train.at(i+1).pt.x;

        bool far_from_next = next_x - current_x > max_dist;
        bool far_from_prev = current_x - prev_x > max_dist;
        bool close_to_next = next_x - current_x < min_dist;
        bool close_to_prev = current_x - prev_x < min_dist;

        if(i==1 && not(far_from_prev || close_to_prev)){
            pos_to_keep.push_back(i-1);
        }
        if(not(far_from_next || far_from_prev || close_to_next || close_to_prev)){
            pos_to_keep.push_back(i);
        }

        if(i==k_train.size()-2 && not(far_from_next || close_to_next)){
            pos_to_keep.push_back(i+1);
        }
    }

    std::vector<KeyPoint> new_k_query, new_k_train;
    std::vector<DMatch> new_good_matches;

    for(int i=0; i<pos_to_keep.size(); i++){
        int pos = pos_to_keep.at(i);
        new_k_query.push_back(k_query.at(pos));
        new_k_train.push_back(k_train.at(pos));
        new_good_matches.push_back(good_matches.at(pos));
    }
    k_query = new_k_query;
    k_train = new_k_train;
    good_matches = new_good_matches;
    
    for(int i=0; i<good_matches.size(); i++){
        good_matches.at(i).queryIdx = i;
        good_matches.at(i).trainIdx = i;
    }

                 
}

bool checkCorners(std::vector<cv::Point2f> src_corners,
                  std::vector<cv::Point2f> dst_corners){
    float sum = 0.0;
    for(int i=0; i < dst_corners.size(); i++){
        Point2f next = dst_corners.at((i+1)%4);
        Point2f curr = dst_corners.at((i)%4);
        sum += (next.x-curr.x)*(next.y+curr.y);
    }
    bool isClockwise = sum<0? true: false;
    return isClockwise;

}
cv::Rect getRectFromCorners(std::vector<cv::Point2f> dst_corners, Size img_size){
    int x = (dst_corners.at(0).x+dst_corners.at(3).x)/2;
    x = x>0? x:0;
    int width = (dst_corners.at(1).x+dst_corners.at(2).x)/2 - x;
    int y = (dst_corners.at(0).y+dst_corners.at(1).y)/2;
    y = y>0? y:0;
    int height = (dst_corners.at(2).y+dst_corners.at(3).y)/2 - y;

    if(x>img_size.width || y >img_size.height){
        return Rect(0, 0, 0, 0);
    }else {
        if(x+width>img_size.width)
            width=img_size.width-x-1;
        if(x<0)
            x=0;
        if(y+height>img_size.height)
            height=img_size.height-y-1;
        if(y<0)
            y=0;
    }

    cv::Rect rect(x, y, width, height);
    return rect;
}

int main( int argc, char* argv[] )
{

    Mat img1 = imread( "samples/filas/prod_samp4/S4_P9.jpg", IMREAD_GRAYSCALE ); // features/S4_P9
    Mat img2 = imread( "samples/filas/sample4_1.jpg", IMREAD_GRAYSCALE );
    if ( img1.empty() || img2.empty() )
    {
        cout << "Could not open or find the image!\n" << endl;
        return -1;
    }
    imshow("1", img1 );
    imshow("2", img2 );
    //-- Step 1: Detect the keypoints using SURF Detector, compute the descriptors
    int minHessian = 400;
    Ptr<SURF> detector = SURF::create( minHessian );
    std::vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;
    int window_width = img2.rows*img1.cols/img1.rows;
    int x_step= 0.25*window_width;

    Rect roi;
    Mat roi_img;
    std::vector<cv::Rect> boxes;
    int width;
    bool cornersChecked = false;
    for(int x_offset=0;  x_offset<img2.cols-0.5*x_step;  x_offset+=x_step){
        if( (img2.cols-x_offset) > window_width ){
            width = window_width;
        } else{
            width = img2.cols-x_offset;
        }

        roi = Rect(x_offset, 0, width, img2.rows);
        if((roi.width + window_width/25 < window_width) && not(cornersChecked)){
            break;
        }
        roi_img = img2(roi);
        imshow("Current Rect", roi_img);
        waitKey();

        detector->detectAndCompute( img1, noArray(), keypoints1, descriptors1 );
        detector->detectAndCompute( roi_img, noArray(), keypoints2, descriptors2 );

        //-- Step 2: Matching descriptor vectors with a FLANN based matcher
        // Since SURF is a floating-point descriptor NORM_L2 is used
        Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
        std::vector< std::vector<DMatch> > knn_matches;
        matcher->knnMatch( descriptors1, descriptors2, knn_matches, 2 );
        //-- Filter matches using the Lowe's ratio test
        const float ratio_thresh = 0.7f;
        std::vector<DMatch> good_matches;
        for (size_t i = 0; i < knn_matches.size(); i++)
        {
            if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
            {
                good_matches.push_back(knn_matches[i][0]);
            }
        }

        std::vector<cv::Point2f> src_pts;
        std::vector<cv::Point2f> dst_pts;
        for(int i=0; i<good_matches.size(); i++){
            if(good_matches.at(i).distance < 0.2){
                int src_id = good_matches.at(i).queryIdx;
                int dst_id = good_matches.at(i).trainIdx;
                src_pts.push_back(keypoints1.at(src_id).pt);
                Point2f dst_pt = keypoints2.at(dst_id).pt;
                dst_pt.x += x_offset;
                dst_pts.push_back(dst_pt);
            }
        }
        if(src_pts.size()<6) continue;
        Mat perspectiveMat_= findHomography(src_pts, dst_pts);

        std::vector<cv::Point2f> src_corners;
        src_corners.push_back(Point2f(0, 0));
        src_corners.push_back(Point2f(img1.cols, 0));
        src_corners.push_back(Point2f(img1.cols, img1.rows));
        src_corners.push_back(Point2f(0, img1.rows));
        std::vector<cv::Point2f> dst_corners;
        perspectiveTransform(src_corners, dst_corners, perspectiveMat_);
        cornersChecked = checkCorners(src_corners, dst_corners);
        if(not(cornersChecked))
            continue;
        cv::Rect box = getRectFromCorners(dst_corners, img2.size());
        imshow("box", img2(box));
        boxes.push_back(box);
        //-- Draw matches
        Mat img_matches;
        drawMatches( img1, keypoints1, roi_img, keypoints2, good_matches, img_matches, Scalar::all(-1),
                 Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
        //-- Show detected matches
        imshow("Good Matches", img_matches );
        
        waitKey();
        if(cornersChecked){
            x_offset = box.x+box.width-x_step;
        }
    }
    return 0;
}

#else
int main()
{
    std::cout << "This tutorial code needs the xfeatures2d contrib module to be run." << std::endl;
    return 0;
}
#endif