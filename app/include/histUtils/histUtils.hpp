#ifndef HISTO_UTILS_
#define HISTO_UTILS_

#include <vector>
#include <opencv2/imgproc/imgproc.hpp>

#define HIST_CALCULATED 0x80
#define HIST_NORMALIZED 0x40
#define HIST_MEAN 0x20
#define HIST_STD_DEV 0x10
#define HIST_EQ_BINS 0x08
#define HIST_EQ_RANGE 0x04

struct HistRange {
    int min;
    int max;
};

class Hist {
    public:
    Hist();

    Hist(const cv::Mat& src_, unsigned int n_bins_ = 16, 
        const cv::Mat& mask_ = cv::Mat(), HistRange range_ = {0, 255});

    Hist(const cv::Mat& src_, std::vector<unsigned int>& n_bins_,
        std::vector<HistRange>& ranges_, const cv::Mat& mask_ = cv::Mat());

    private:
    cv::Mat src;
    cv::Mat mask;
    cv::Mat hist;
    cv::Mat norm_hist;
    unsigned int dims;
    std::vector<unsigned int> bins;
    std::vector<HistRange> ranges;
    std::vector<double> mean;
    std::vector<double> stdDev;
    unsigned char flags;
    void calcHistogram();
    void normalizeHist();
    void calculateMean();

    // Helper functions
    std::vector<double> getDimHisto(int dim);
    double getMean(const std::vector<double>& src);
};

#endif