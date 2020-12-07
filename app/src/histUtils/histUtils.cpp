#include <histUtils/histUtils.hpp>

Hist::Hist()
    : dims(0), flags(0)
{
}

Hist::Hist(const cv::Mat& src_, unsigned int n_bins_,
    const cv::Mat& mask_, HistRange range_)
    : dims(src.channels()), flags(0), src(src), mask(mask_)
{
    for (int i = 0; i < dims; ++i)
    {
        bins.push_back(n_bins_);
        ranges.push_back(range_);
    }

    flags |= HIST_EQ_BINS + HIST_EQ_RANGE;
}

Hist::Hist(const cv::Mat& src_, std::vector<unsigned int>& n_bins_,
        std::vector<HistRange>& ranges_, const cv::Mat& mask_)
    : dims(src.channels()), flags(0), src(src_), 
        mask(mask_), bins(n_bins_), ranges(ranges_)
{
}

void Hist::calcHistogram()
{
    if (src.empty() || dims == 0) {
        return;
    }
    
    flags &= ~(HIST_MEAN + HIST_STD_DEV + HIST_NORMALIZED);

    int channels[dims];
    float* ranges_[dims];
    float range_[dims*2];
    for (int i = 0; i < dims; ++i)
    {
        range_[i*2] = ranges[i].min;
        range_[i*2 + 1] = ranges[i].max;
        channels[i] = i;
        ranges_[i] = range_ + i*2; 
    }

    cv::calcHist(
        &src, 1, channels, mask,
        hist, dims, bins.data(), ranges_,
        true, // bins uniformes
        false // no se acumula
    );

    flags |= HIST_CALCULATED;
}

void Hist::normalizeHist()
{
    cv::normalize(hist, norm_hist, 1, 0, cv::NORM_L1);
    flags |= HIST_NORMALIZED;
}

void Hist::calculateMean()
{
    if ((flags & HIST_NORMALIZED) == 0)
        normalizeHist();

    for (int i = 0; i < dims; ++i)
    {
        std::vector<double> dimHisto = getDimHisto(i);
        mean.push_back(getMean(dimHisto));
    }
    
}