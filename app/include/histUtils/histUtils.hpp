#ifndef HISTO_UTILS_
#define HISTO_UTILS_

#include <vector>
#include <opencv2/imgproc/imgproc.hpp>

#define HIST_CALCULATED 0x80
#define HIST_NORMALIZED 0x40
#define HIST_MEAN 0x20
#define HIST_STD_DEV 0x10
#define HIST_CHANNELS 0x08

struct HistRange {
    int min;
    int max;
};

class Hist {
    public:
    Hist();

    Hist(const cv::Mat& src_, int n_bins_ = 16, 
        const cv::Mat& mask_ = cv::Mat(), HistRange range_ = {0, 255});

    Hist(const cv::Mat& src_, std::vector<int>& n_bins_,
        std::vector<HistRange>& ranges_, const cv::Mat& mask_ = cv::Mat());

    void loadSrc(const cv::Mat& src_, std::vector<int> &n_bins_,
        std::vector<HistRange> &ranges_, const cv::Mat& mask_ = cv::Mat());

    cv::Mat getHist();
    cv::Mat getNormalizedHist();
    double calcDistance(Hist& other);
    std::vector<double> getMean();
    std::vector<double> getStdDev();

    private:
    cv::Mat src;
    cv::Mat mask;
    cv::Mat hist;
    cv::Mat norm_hist;
    std::vector<cv::Mat> channelsHist;

    std::vector<int> bins;
    std::vector<HistRange> ranges;
    std::vector<double> mean;
    std::vector<double> stdDev;

    int dims;
    unsigned char flags;

    void calcHistogram();
    void calcChannelsHist();
    void normalizeHist();
    void calcMeanStdDev();

    // disable =operator & copy constructor
    Hist(const Hist& other);
    Hist& operator=(const Hist& other);

};

#endif