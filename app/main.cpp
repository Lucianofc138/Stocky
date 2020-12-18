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


struct MatchInfo {
    std::vector<KeyPoint> keypts_temp;
    std::vector<KeyPoint> keypts_roi;
    std::vector<DMatch> good_matches;
};

struct RelevantPoints {
    std::vector<cv::Point2f> src_pts;
    std::vector<cv::Point2f> dst_pts;
};

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

void filterGoodMatches(std::vector< std::vector<DMatch> >& knn_matches,
                   std::vector<DMatch>& dst_, double threshold = 0.7)
{
    for (size_t i = 0; i < knn_matches.size(); i++)
    {
        if (knn_matches[i][0].distance < threshold * knn_matches[i][1].distance)
        {
            dst_.push_back(knn_matches[i][0]);
        }
    }
}



void featureMatching(cv::Mat templateImg, cv::Mat srcImg, MatchInfo& match){
    int minHessian = 400;
    Ptr<SURF> detector = SURF::create( minHessian );

    Mat descriptors1, descriptors2;

    detector->detectAndCompute( templateImg, noArray(), match.keypts_temp, descriptors1 );
    detector->detectAndCompute( srcImg, noArray(), match.keypts_roi, descriptors2 );

    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    std::vector< std::vector<DMatch> > knn_matches;
    matcher->knnMatch( descriptors1, descriptors2, knn_matches, 2 );

    const float ratio_thresh = 0.7f;

    filterGoodMatches(knn_matches, match.good_matches, ratio_thresh);
}

void removeMatch(MatchInfo& match, Point2d point, int ksize){
    std::vector<int> indexToDelete; 
    for(int i=0; i< match.good_matches.size(); i++){
        int matchId = match.good_matches.at(i).trainIdx;
        Point2d it_pt = match.keypts_roi.at(matchId).pt;
        if(cv::norm(point-it_pt) < 1 )
            indexToDelete.push_back(i);
    }
    while( indexToDelete.size() > 0 ){
        int index = indexToDelete.back();
        match.good_matches.erase( match.good_matches.begin() + index );
        indexToDelete.pop_back();
    }
}

void filterMatchesByDistance(MatchInfo& match, Size roiSize, int kernel_size){
    cv::Mat roiKeyPts(roiSize, CV_8UC1, Scalar(0,0,0));
    for(int i=0; i<match.good_matches.size(); ++i)
    {
        int keypt_id = match.good_matches.at(i).trainIdx;
        cv::Point2d roi_keypt = match.keypts_roi.at(keypt_id).pt;
        roiKeyPts.at<uchar>(roi_keypt) += 1;
    }
    // imshow("KEY POINTS", roiKeyPts);
    for(int j=0; j < roiKeyPts.rows-kernel_size; j++) {
        for(int i=0; i < roiKeyPts.cols-kernel_size; i++) {
            uchar cumsum = 0;


            for (int k = 0; k < kernel_size; ++k)
            {
                for (int h = 0; h < kernel_size; ++h)
                    cumsum += roiKeyPts.at<uchar>(i + k, j + h);
            }

            if (cumsum > 1) {
                for (int k = 0; k < kernel_size; ++k)
                {
                    for (int h = 0; h < kernel_size; ++h)
                        removeMatch(match, Point2d(i+k, j+h), kernel_size);
                } 
            }
        }
    }
}

void getRelevantPoints(MatchInfo& match, RelevantPoints& pts, int x_offset)
{
    std::vector <int> indexToDelete;
    for(int i=0; i<match.good_matches.size(); i++){
        if(match.good_matches.at(i).distance < 0.15){
            int src_id = match.good_matches.at(i).queryIdx;
            int dst_id = match.good_matches.at(i).trainIdx;
            pts.src_pts.push_back(match.keypts_temp.at(src_id).pt);
            Point2f dst_pt = match.keypts_roi.at(dst_id).pt;
            dst_pt.x += x_offset;
            pts.dst_pts.push_back(dst_pt);
        }else{
            indexToDelete.push_back(i);
        }
    }
    while( indexToDelete.size() > 0 ){
        int index = indexToDelete.back();
        match.good_matches.erase( match.good_matches.begin() + index );
        indexToDelete.pop_back();
    }

}

void filterBestPoints(MatchInfo& match, int pointsAmount=6)
{
    std::vector <int> indexToKeep;
    std::vector <int> indexToDelete;
    for(int j=0; j<pointsAmount; j++){
        float minDist = 1;
        int minDistIndex;
        for(int i=0; i<match.good_matches.size(); i++){
            bool skip = false;
            for(int k=0; k<indexToKeep.size(); k++){
                if(i==indexToKeep.at(k))
                    skip = true;
            }
            if (skip)
                continue;
            float dist = match.good_matches.at(i).distance;
            if (dist < minDist){
                minDist = dist;
                minDistIndex = i;
            }
        }
        indexToKeep.push_back(minDistIndex);       
    }
    for(int i=0; i<match.good_matches.size(); i++){
        bool skip = false;
        for(int k=0; k<indexToKeep.size(); k++){
            if(i==indexToKeep.at(k))
                skip = true;
        }
        if (skip)
            continue;
        indexToDelete.push_back(i);
    }
    while( indexToDelete.size() > 0 ){
        int index = indexToDelete.back();
        match.good_matches.erase( match.good_matches.begin() + index );
        indexToDelete.pop_back();
    }
}

void getCornerPointsCW(cv::Mat img, std::vector<cv::Point2f>& corners)
{
    corners.push_back(Point2f(0, 0));
    corners.push_back(Point2f(img.cols, 0));
    corners.push_back(Point2f(img.cols, img.rows));
    corners.push_back(Point2f(0, img.rows));
}
void scanFeaturesSlidingWindow(cv::Mat templateImg, cv::Mat floorImg, std::vector<cv::Rect> boxes)
{
    int window_width = floorImg.rows*templateImg.cols/templateImg.rows;
    int x_step= 0.25*window_width;

    cv::Rect roi;
    cv::Mat roi_img;
    int width;
    bool cornersChecked = false;
    for(int x_offset=0;  x_offset<floorImg.cols-0.5*x_step;  x_offset+=x_step){
        if( (floorImg.cols-x_offset) > window_width ){
            width = window_width;
        } else {
            width = floorImg.cols-x_offset;
        }

        roi = Rect(x_offset, 0, width, floorImg.rows);
        if((roi.width + window_width/25 < window_width) && not(cornersChecked)){
            break;
        }
        roi_img = floorImg(roi);
        // imshow("Current Rect", roi_img);
        // cv::waitKey(0);

        MatchInfo match;
        RelevantPoints relevantPts;
        featureMatching(templateImg, roi_img, match);
        filterMatchesByDistance(match, roi_img.size(), 9);
        filterBestPoints(match,  6);
        getRelevantPoints(match, relevantPts, x_offset);
        int minKeypointsAmount = 6;
        if(relevantPts.src_pts.size() < minKeypointsAmount)
            continue;
        Mat perspectiveMat_= findHomography(relevantPts.src_pts, relevantPts.dst_pts);
        std::vector<cv::Point2f> templateCorners, dstCorners;
        getCornerPointsCW(templateImg, templateCorners);
        perspectiveTransform(templateCorners, dstCorners, perspectiveMat_);
        cornersChecked = checkCorners(templateCorners, dstCorners);
        
        if(not(cornersChecked))
            continue;
        cv::Rect box = getRectFromCorners(dstCorners, floorImg.size());
        imshow("box", floorImg(box));
        boxes.push_back(box);
        Mat img_matches;
        drawMatches( templateImg, match.keypts_temp, roi_img, match.keypts_roi, match.good_matches,
                     img_matches, Scalar::all(-1), Scalar::all(-1), std::vector<char>(),
                     DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
        imshow("Good Matches", img_matches );
        cv::waitKey(0);

        if(cornersChecked){
            x_offset = box.x+box.width-x_step;
        }
    }
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

    std::vector<cv::Rect> boxes;
    scanFeaturesSlidingWindow(img1, img2, boxes);

    return 0;
}

#else
int main()
{
    std::cout << "This tutorial code needs the xfeatures2d contrib module to be run." << std::endl;
    return 0;
}
#endif