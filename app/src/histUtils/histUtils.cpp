#include <histUtils/histUtils.hpp>

Hist::Hist()
    : dims(0), flags(0)
{
}

Hist::Hist(const cv::Mat &src_, int n_bins_,
           const cv::Mat &mask_, HistRange range_)
    : dims(src.channels()), flags(0), src(src_),
        mask(mask_), normChannelsHist(src.channels()),
        bins(src.channels()), ranges(src.channels()),
        mean(src.channels()), stdDev(src.channels())
{
    for (int i = 0; i < dims; ++i)
    {
        bins[i] = n_bins_;
        ranges[i] = range_;
    }
}

Hist::Hist(const cv::Mat &src_, std::vector<int> &n_bins_,
           std::vector<HistRange> &ranges_, const cv::Mat &mask_)
    : dims(src.channels()), flags(0), src(src_),
      mask(mask_), bins(n_bins_), ranges(ranges_), normChannelsHist(src.channels()),
      mean(src.channels()), stdDev(src.channels())
{
}

void Hist::loadSrc(const cv::Mat& src_, std::vector<int> &n_bins_,
        std::vector<HistRange> &ranges_, const cv::Mat& mask_)
{
    if (src_.empty())
        return;

    flags = 0;

    src = src_;
    mask = mask_;
    dims = src_.channels();
    bins = n_bins_;
    ranges = ranges_;
    
    normChannelsHist.resize(dims);
    bins.resize(dims);
    ranges.resize(dims);
    mean.resize(dims);
    stdDev.resize(dims);
}

void Hist::calcHistogram()
{
    if (src.empty() || dims == 0)
    {
        return;
    }

    flags &= ~(HIST_MEAN + HIST_STD_DEV + HIST_NORMALIZED + HIST_CHANNELS);

    int channels[dims];
    float *ranges_[dims];
    float range_[dims * 2];
    for (int i = 0; i < dims; ++i)
    {
        range_[i * 2] = (float)ranges[i].min;
        range_[i * 2 + 1] = (float)ranges[i].max;
        channels[i] = i;
        ranges_[i] = range_ + i * 2;
    }

    const int* c_bins = bins.data();
    const float** c_ranges = (const float**)ranges_;

    cv::calcHist(
        &src, 1, channels, mask,
        hist, dims, c_bins, c_ranges,
        true, // bins uniformes
        false // no se acumula
    );

    flags |= HIST_CALCULATED;
}

void Hist::calcChannelsHist()
{
    if (src.empty() || dims == 0)
    {
        return;
    }

    std::vector<cv::Mat> channels;
    cv::split(src, channels);

    for (int i = 0; i < channels.size(); ++i)
    {
        float range[2] = {(float)ranges[i].min, (float)ranges[i].max};
        float* histRange = { range };

        const int* c_bins = &bins[i];
        const float** c_ranges = (const float**)&histRange;

        cv::calcHist(
            &channels[i], 1, 0, mask,
            normChannelsHist[i], 1, c_bins, c_ranges,
            true, // bins uniformes
            false // no se acumula
        );
        cv::normalize(normChannelsHist[i],
            normChannelsHist[i], 1, 0, cv::NORM_L1);
    }

    flags |= HIST_CHANNELS;
}

void Hist::normalizeHist()
{
    if ((flags & HIST_CALCULATED) == 0)
        calcHistogram();
    cv::normalize(hist, norm_hist, 1, 0, cv::NORM_L1);
    flags |= HIST_NORMALIZED;
}

void Hist::calculateMeanStdDev()
{
    if ((flags & HIST_CHANNELS) == 0)
        calcChannelsHist();

    for (int i = 0; i < normChannelsHist.size(); ++i)
    {
        cv::Scalar mean_, stdDev_;
        cv::meanStdDev(normChannelsHist[i], mean_, stdDev_);
        mean[i] = mean_[0];
        stdDev[i] = stdDev_[0];
    }

    flags |= HIST_MEAN + HIST_STD_DEV;
}

cv::Mat Hist::getHistogram()
{
    return hist.clone();
}

std::vector<double> Hist::getMean()
{
    return mean;
}

std::vector<double> Hist::getStdDev()
{
    return stdDev;
}