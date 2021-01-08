#include "processingMisc/processingMisc.hpp"

namespace stky
{
    RelevantFrames::RelevantFrames( cv::Mat bg_  )
    {
        this->frameNumber = 0;
    }

    void RelevantFrames::initMog()
    {
        bgSustractor.mog = cv::createBackgroundSubtractorMOG2(
            bgSustractor.historyMOG, bgSustractor.varThreshold, bgSustractor.bShadowDetection);
        bgSustractor.mog->setNMixtures(bgSustractor.nmixtures);
        bgSustractor.mog->setBackgroundRatio(bgSustractor.backgroundRatio);
    }

    cv::Mat RelevantFrames::getFg()
    {
        return this->fg;
    }

    void RelevantFrames::setFrame(cv::Mat frame_)
    {
        this->frame = frame_;
    }

    void RelevantFrames::calcFg()
    {
        if (this->frameNumber == 0 && this->bg.empty())
        {
            frame.copyTo(bg);
        }
        else
        {
            if (this->frameNumber < 60)
                bgSustractor.mog->apply(bg, fg, bgSustractor.learningRate);
            else
                bgSustractor.mog->apply(frame, fg, bgSustractor.learningRate);
            cv::threshold(fg, fg, 175, 255, cv::THRESH_BINARY);
            getMovementMask();
        }
        this->frameNumber++;
    }

    void RelevantFrames::getMovementMask()
    {
    // cv::bitwise_not(fg, fg);
    cv::medianBlur(fg, fg, 11);
    cv::erode(fg, fg, cv::getStructuringElement(cv::MORPH_ELLIPSE, 
                             cv::Size(frame.cols*0.05, frame.cols*0.05)));

    cv::Mat flood = fg.clone();
    cv::floodFill(flood, cv::Point2d(1, 1), cv::Scalar(255, 255, 255));
    cv::bitwise_not(flood, flood);
    cv::dilate(flood, flood, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
    cv::bitwise_or(fg, flood, fg);

    cv::dilate(fg, fg, cv::getStructuringElement(
        cv::MORPH_ELLIPSE, cv::Size(frame.cols*0.08, frame.rows*0.08)));
    }

}