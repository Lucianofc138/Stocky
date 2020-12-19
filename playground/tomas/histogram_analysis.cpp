#include <histUtils/histUtils.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <iterator>

int main() {
    cv::Mat src = cv::imread("samples/producto/trix/trix1.png", cv::IMREAD_COLOR);

    cv::Mat yCrCbSrc, crCbSrc;
    cv::cvtColor(src, yCrCbSrc, cv::COLOR_BGR2YCrCb);

    std::vector<cv::Mat> channels;

    cv::split(yCrCbSrc, channels);

    channels.erase(channels.begin());

    cv::merge(channels, crCbSrc);

    Hist chocapicHisto(crCbSrc);

    cv::Mat histo = chocapicHisto.getHist();
    cv::Mat normHist = chocapicHisto.getNormalizedHist();
    std::vector<double> mean = chocapicHisto.getMean();
    std::vector<double> stdDev = chocapicHisto.getStdDev();

    std::cout << "Histogram: " << histo << std::endl;
    std::cout << "Normalized Histogram: " << normHist << std::endl;
    
    std::cout << "Mean: ";
    std::copy(mean.begin(),
            mean.end(),
            std::ostream_iterator<double>(std::cout, " "));
    std::cout << std::endl;

    std::cout << "stdDev: ";
    std::copy(stdDev.begin(),
            stdDev.end(),
            std::ostream_iterator<double>(std::cout, " "));
    std::cout << std::endl;

    cv::imshow("original", src);
    cv::waitKey(0);
}