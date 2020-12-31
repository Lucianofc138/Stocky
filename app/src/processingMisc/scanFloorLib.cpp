#include "processingMisc/processingMisc.hpp"

#define TOP_LEFT 0
#define TOP_RIGHT 1
#define BOT_RIGHT 2
#define BOT_LEFT 3
namespace stky
{


    bool checkCorners(std::vector<cv::Point2f> src_corners,
                      std::vector<cv::Point2f> dst_corners)
    {
        float sum = 0.0;
        for (int i = 0; i < dst_corners.size(); i++)
        {
            cv::Point2f next = dst_corners.at((i + 1) % 4);
            cv::Point2f curr = dst_corners.at((i) % 4);
            sum += (next.x - curr.x) * (next.y + curr.y);
        }
        bool isClockwise = sum < 0 ? true : false;
        return isClockwise;
    }

    cv::Rect getRectFromCorners(std::vector<cv::Point2f> dst_corners, cv::Size img_size)
    {
        bool x0Deflected = 
            abs(dst_corners.at(TOP_LEFT).x - dst_corners.at(BOT_LEFT).x) > 0.05*img_size.width;
        bool x1Deflected = 
            abs(dst_corners.at(TOP_RIGHT).x - dst_corners.at(BOT_RIGHT).x) > 0.05*img_size.width;
        bool y0Deflected = 
            abs(dst_corners.at(TOP_LEFT).y - dst_corners.at(TOP_RIGHT).y) > 0.5*img_size.height;
        bool y1Deflected = 
            abs(dst_corners.at(BOT_LEFT).y - dst_corners.at(BOT_RIGHT).y) > 0.5*img_size.height;
        
        if (x0Deflected || x1Deflected || y0Deflected || y1Deflected){
            return cv::Rect();
        }

        int x = (dst_corners.at(0).x + dst_corners.at(3).x) / 2;
        x = x > 0 ? x : 0;
        int width = (dst_corners.at(1).x + dst_corners.at(2).x) / 2 - x;
        int y = (dst_corners.at(0).y + dst_corners.at(1).y) / 2;
        y = y > 0 ? y : 0;
        int height = (dst_corners.at(2).y + dst_corners.at(3).y) / 2 - y;

        if (x > img_size.width || y > img_size.height)
        {
            return cv::Rect(0, 0, 0, 0);
        }
        else
        {
            if (x + width > img_size.width)
                width = img_size.width - x - 1;
            if (x < 0)                             // ??
                x = 0;
            height = img_size.height - y - 1;
            if (y < 0)                             // ??
                y = 0;
        }
        if (width<1 || height<1)
            return cv::Rect();
        else
            return cv::Rect(x, y, width, height);
    }

    void filterGoodMatches(std::vector<std::vector<cv::DMatch>> &knn_matches,
                           std::vector<cv::DMatch> &dst_, double threshold = 0.7)
    {
        for (size_t i = 0; i < knn_matches.size(); i++)
        {
            if (knn_matches[i][0].distance < threshold * knn_matches[i][1].distance)
            {
                dst_.push_back(knn_matches[i][0]);
            }
        }
    }

    void featureMatching(cv::Mat templateImg, cv::Mat srcImg, MatchInfo &match)
    {
        int minHessian = 400;
        cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(minHessian);

        cv::Mat descriptors1, descriptors2;

        detector->detectAndCompute(templateImg, cv::noArray(), match.keypts_temp, descriptors1);
        detector->detectAndCompute(srcImg, cv::noArray(), match.keypts_roi, descriptors2);

        cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
        std::vector<std::vector<cv::DMatch>> knn_matches;
        matcher->knnMatch(descriptors1, descriptors2, knn_matches, 2);

        const float ratio_thresh = 0.7f;

        filterGoodMatches(knn_matches, match.good_matches, ratio_thresh);
    }

    void removeMatch(MatchInfo &match, cv::Point2d point, int ksize)
    {
        std::vector<int> indexToDelete;
        for (int i = 0; i < match.good_matches.size(); i++)
        {
            int matchId = match.good_matches.at(i).trainIdx;
            cv::Point2d it_pt = match.keypts_roi.at(matchId).pt;
            if (cv::norm(point - it_pt) < 1)
                indexToDelete.push_back(i);
        }
        while (indexToDelete.size() > 0)
        {
            int index = indexToDelete.back();
            match.good_matches.erase(match.good_matches.begin() + index);
            indexToDelete.pop_back();
        }
    }

    void filterMatchesByDistance(MatchInfo &match, cv::Size roiSize, int kernel_size)
    {
        cv::Mat roiKeyPts(roiSize, CV_8UC1, cv::Scalar(0, 0, 0));
        for (int i = 0; i < match.good_matches.size(); ++i)
        {
            int keypt_id = match.good_matches.at(i).trainIdx;
            cv::Point2d roi_keypt = match.keypts_roi.at(keypt_id).pt;
            roiKeyPts.at<uchar>(roi_keypt) += 1;
        }
        // imshow("KEY POINTS", roiKeyPts);
        for (int j = 0; j < roiKeyPts.rows - kernel_size; j++)
        {
            for (int i = 0; i < roiKeyPts.cols - kernel_size; i++)
            {
                uchar cumsum = 0;

                for (int k = 0; k < kernel_size; ++k)
                {
                    for (int h = 0; h < kernel_size; ++h)
                        cumsum += roiKeyPts.at<uchar>(i + k, j + h);
                }

                if (cumsum > 1)
                {
                    for (int k = 0; k < kernel_size; ++k)
                    {
                        for (int h = 0; h < kernel_size; ++h)
                            removeMatch(match, cv::Point2d(i + k, j + h), kernel_size);
                    }
                }
            }
        }
    }

    void getRelevantPoints(MatchInfo &match, RelevantPoints &pts, int x_offset)
    {
        for (int i = 0; i < match.good_matches.size(); i++)
        {
            if (match.good_matches.at(i).distance < 0.2)
            {
                int src_id = match.good_matches.at(i).queryIdx;
                int dst_id = match.good_matches.at(i).trainIdx;
                pts.src_pts.push_back(match.keypts_temp.at(src_id).pt);
                cv::Point2f dst_pt = match.keypts_roi.at(dst_id).pt;
                dst_pt.x += x_offset;
                pts.dst_pts.push_back(dst_pt);
            }
        }
    }

    void filterBestPoints(MatchInfo &match, int pointsAmount = 6)
    {
        std::vector<int> indexToKeep;
        std::vector<int> indexToDelete;
        for (int j = 0; j < pointsAmount; j++)
        {
            float minDist = 1;
            int minDistIndex;
            for (int i = 0; i < match.good_matches.size(); i++)
            {
                bool skip = false;
                for (int k = 0; k < indexToKeep.size(); k++)
                {
                    if (i == indexToKeep.at(k))
                        skip = true;
                }
                if (skip)
                    continue;
                float dist = match.good_matches.at(i).distance;
                if (dist < minDist)
                {
                    minDist = dist;
                    minDistIndex = i;
                }
            }
            indexToKeep.push_back(minDistIndex);
        }
        for (int i = 0; i < match.good_matches.size(); i++)
        {
            bool skip = false;
            for (int k = 0; k < indexToKeep.size(); k++)
            {
                if (i == indexToKeep.at(k))
                    skip = true;
            }
            if (skip)
                continue;
            indexToDelete.push_back(i);
        }
        while (indexToDelete.size() > 0)
        {
            int index = indexToDelete.back();
            match.good_matches.erase(match.good_matches.begin() + index);
            indexToDelete.pop_back();
        }
    }

    void getCornerPointsCW(cv::Mat img, std::vector<cv::Point2f> &corners)
    {
        corners.push_back(cv::Point2f(0, 0));
        corners.push_back(cv::Point2f(img.cols, 0));
        corners.push_back(cv::Point2f(img.cols, img.rows));
        corners.push_back(cv::Point2f(0, img.rows));
    }

    void scanFeaturesSlidingWindow(cv::Mat templateImg, cv::Mat floorImg, std::vector<cv::Rect> &boxes)
    {
        int window_width = floorImg.rows * templateImg.cols / templateImg.rows;
        int x_step = 0.25 * window_width;

        cv::Rect roi;
        cv::Mat roi_img;
        int width;
        bool cornersChecked = false;
        for (int x_offset = 0; x_offset < floorImg.cols - 1.1*x_step; x_offset += x_step)
        {
            if ((floorImg.cols - x_offset) > window_width)
            {
                width = window_width;
            }
            else
            {
                width = floorImg.cols - x_offset;
            }

            roi = cv::Rect(x_offset, 0, width, floorImg.rows);
            if ((roi.width + window_width / 25 < window_width) && not(cornersChecked))
            {
                break;
            }
            roi_img = floorImg(roi);
            // imshow("Current Rect", roi_img);
            // cv::waitKey(0);

            MatchInfo match;
            RelevantPoints relevantPts;
            featureMatching(templateImg, roi_img, match);
            filterMatchesByDistance(match, roi_img.size(), 9);
            filterBestPoints(match, 6);
            getRelevantPoints(match, relevantPts, x_offset);
            int minKeypointsAmount = 6;
            if (relevantPts.src_pts.size() < minKeypointsAmount)
                continue;
            cv::Mat perspectiveMat_ = cv::findHomography(relevantPts.src_pts, relevantPts.dst_pts);
            std::vector<cv::Point2f> templateCorners, dstCorners;
            getCornerPointsCW(templateImg, templateCorners);
            perspectiveTransform(templateCorners, dstCorners, perspectiveMat_);
            cornersChecked = checkCorners(templateCorners, dstCorners);
            if (not(cornersChecked))
                continue;

            cv::Mat img_matches;
            drawMatches(templateImg, match.keypts_temp, roi_img, match.keypts_roi, match.good_matches,
                        img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(),
                        cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
            // cv::imshow("Good Matches", img_matches);
            // cv::waitKey(0);
            cv::Rect box = getRectFromCorners(dstCorners, floorImg.size());
            if (box.empty())
                continue;
            imshow("box", floorImg(box));
            boxes.push_back(box);
            // cv::Mat img_matches;
            // drawMatches(templateImg, match.keypts_temp, roi_img, match.keypts_roi, match.good_matches,
            //             img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(),
            //             cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
            cv::imshow("Good Matches", img_matches);
            // cv::waitKey(0);

            if (cornersChecked)
            {
                x_offset = box.x + box.width - x_step;
            }
        }
    }

} // namespace stky