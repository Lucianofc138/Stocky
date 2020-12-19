#include <histUtils/histUtils.hpp>

Hist::Hist()
    : dims(0), flags(0)
{
}

Hist::Hist(const cv::Mat &src_, int n_bins_,
           const cv::Mat &mask_, HistRange range_)
    : dims(src_.channels()), flags(0), src(src_),
        mask(mask_), channelsHist(src_.channels()),
        bins(src_.channels()), ranges(src_.channels()),
        mean(src_.channels()), stdDev(src_.channels())
{
    for (int i = 0; i < dims; ++i)
    {
        bins[i] = n_bins_;
        ranges[i] = range_;
    }
}

Hist::Hist(const cv::Mat &src_, std::vector<int> &n_bins_,
           std::vector<HistRange> &ranges_, const cv::Mat &mask_)
    : dims(src_.channels()), flags(0), src(src_),
      mask(mask_), bins(n_bins_), ranges(ranges_), channelsHist(src_.channels()),
      mean(src_.channels()), stdDev(src_.channels())
{
}

Hist::Hist(const Hist& other)
{
    flags = 0;
    dims = 0;
    if (other.src.empty())
        return;

    setMembers(other);
    resizeHist();
}

Hist& Hist::operator=(const Hist& other)
{
    flags = 0;
    dims = 0;
    if (other.src.empty())
        return *this;

    setMembers(other);
    resizeHist();
    return *this;
}

void Hist::loadSrc(const cv::Mat& src_, std::vector<int> &n_bins_,
        std::vector<HistRange> &ranges_, const cv::Mat& mask_)
{
    flags = 0;
    dims = 0;
    if (src_.empty())
        return;

    src = src_;
    mask = mask_;
    dims = src_.channels();
    bins = n_bins_;
    ranges = ranges_;
    
    resizeHist();
}

void Hist::calcHistogram()
{
    if (src.empty() || dims == 0)
        return;

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
        return;

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
            channelsHist[i], 1, c_bins, c_ranges,
            true, // bins uniformes
            false // no se acumula
        );
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

void Hist::calcMeanStdDev()
{
    if (src.empty() || dims == 0)
        return;

    std::vector<cv::Mat> channels;
    cv::split(src, channels);

    for (int i = 0; i < channels.size(); ++i)
    {
        cv::Scalar mean_, stdDev_;
        cv::meanStdDev(channels[i], mean_, stdDev_);
        mean[i] = (mean_[0] - ranges[i].min)/(ranges[i].max - ranges[i].min);
        stdDev[i] = stdDev_[0]/mean_[0];
    }

    flags |= HIST_MEAN + HIST_STD_DEV;
}

cv::Mat Hist::getHist()
{
    if ((flags & HIST_CALCULATED) == 0)
        calcHistogram();
    return hist.clone();
}

cv::Mat Hist::getNormalizedHist()
{
    if ((flags & HIST_NORMALIZED) == 0)
        normalizeHist();
    return norm_hist.clone();
}

double Hist::calcDistance(Hist& other, int method)
{
    if ((flags & HIST_NORMALIZED) == 0)
        normalizeHist();
    return cv::compareHist(norm_hist, other.getNormalizedHist(), method);
}

std::vector<double> Hist::getMean()
{
    if ((flags & HIST_MEAN) == 0)
        calcMeanStdDev();
    return mean;
}

std::vector<double> Hist::getStdDev()
{
    if ((flags & HIST_STD_DEV) == 0)
        calcMeanStdDev();
    return stdDev;
}

void Hist::setMembers(const Hist& other)
{
    src = other.src;
    mask = other.mask;
    dims = other.src.channels();
    bins = other.bins;
    ranges = other.ranges;
}

void Hist::resizeHist()
{
    channelsHist.resize(dims);
    bins.resize(dims);
    ranges.resize(dims);
    mean.resize(dims);
    stdDev.resize(dims);
}