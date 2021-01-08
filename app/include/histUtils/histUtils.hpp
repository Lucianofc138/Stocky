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

    /**
    * Constructor de Hist
    */
    Hist();

    /**
     * Constructor de Hist
     * @param src_ imagen para calcular histograma
     * @param n_bins_ numero de bins por defecto 16
     * @param mask_ matriz con la mascara vacia por defecto
     * @return range_ rango del histograma por defecto
     */
    Hist(const cv::Mat& src_, int n_bins_ = 16, 
        const cv::Mat& mask_ = cv::Mat(), HistRange range_ = {0, 255});

    /**
     * Constructor de Hist
     * @param src_ imagen para calcular histograma
     * @param n_bins_ numero de bins
     * @param mask_ matriz con la mascara
     * @return range_ rango del histograma
     */
    Hist(const cv::Mat& src_, std::vector<int>& n_bins_,
        std::vector<HistRange>& ranges_, const cv::Mat& mask_ = cv::Mat());

    /**
     * Carga variables al histograma
     * @param src_ imagen para calcular histograma
     * @param n_bins_ numero de bins
     * @param mask_ matriz con la mascara
     * @return range_ rango del histograma
     */
    void loadSrc(const cv::Mat& src_, std::vector<int> &n_bins_,
        std::vector<HistRange> &ranges_, const cv::Mat& mask_ = cv::Mat());

    /**
     * @ return matriz con el histograma 
     */
    cv::Mat getHist();

    /**
     * @return Matriz normalizada del histograma
     */
    cv::Mat getNormalizedHist();

    /**
     * Calcula la distancia de BHATTACHARYYA
     * @param other histograma a comparar
     * @return un numero decimal con la comparacion entre histogramas
     */
    double calcDistance(Hist& other);

    /**
     * @return vector de promedio para el histograma
     */
    std::vector<double> getMean();

    /**
     * @return vector de desviacion estandar para histograma
     */
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