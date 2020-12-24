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
    cv::bitwise_not(fg, fg);
    cv::dilate(fg, fg, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(frame.cols / 70, frame.rows / 35)));
    cv::erode(fg, fg, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(frame.cols / 14, frame.rows / 7)));
    cv::dilate(fg, fg, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(frame.cols / 100, frame.rows / 50)));
    cv::bitwise_not(fg, fg);
    }

}