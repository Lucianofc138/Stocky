#include <histUtils/histUtils.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>

#include <CSVWriter.h>

const std::vector<char> letters = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 
                        'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
                        'q', 'r', 's', 't', 'u', 'v'};
const std::vector<int> histMethods = {cv::HISTCMP_BHATTACHARYYA, cv::HISTCMP_CHISQR,
                            cv::HISTCMP_CORREL, cv::HISTCMP_CHISQR_ALT,
                            cv::HISTCMP_INTERSECT, cv::HISTCMP_KL_DIV};
const std::vector<std::string> methodNames = {
    std::string("Bhattacharyya"), std::string("Chisquare"), std::string("Correlation"),
    std::string("Chisquare Alt"), std::string("Intersect"), std::string("KL DIV")
};

const int NUM_PRODUCTS = letters.size()*2;
const int MIN_BIN = 1;
const int MAX_BIN = 255;

void preprocessImage(cv::Mat& src_, cv::Mat& dst_)
{
    dst_ = src_;
}

void getSubMat(cv::Mat& src_, cv::Mat& dst_, std::vector<int> channels_)
{
    std::vector<cv::Mat> channels;
    std::vector<cv::Mat> filteredChannels;
    cv::split(src_, channels);

    for (int i = 0; i < channels_.size(); ++i)
        filteredChannels.push_back(channels[channels_[i]]);

    cv::merge(filteredChannels, dst_);
}

int main()
{
    CSVWriter csv1;
    CSVWriter csv2;

    csv1.newRow() << "group_num" << "num_product" << "mean_0" << "mean_1" << "stddev_0" << "stddev_1";
    csv2.newRow() << "group_num" << "n_bins" << "distance" << "method";

    std::stringstream path;
    std::vector<Hist> groupModel;

    int group = 0;
    for (int i = 0; i < NUM_PRODUCTS; ++i)
    {
        path << "playground/tomas/samples/diff_products/" << 
            letters[group] << std::to_string(i%2 + 1) << ".jpg";
        std::string filename = path.str();

        cv::Mat temp = cv::imread(filename, cv::IMREAD_COLOR);
        preprocessImage(temp, temp);
        cv::cvtColor(temp, temp, cv::COLOR_BGR2Lab);
        std::vector<int> channels = {1, 2};
        getSubMat(temp, temp, channels);

        for (int j = MIN_BIN; j <= MAX_BIN; ++j)
        {
            Hist tempHist(temp, j);

            if (j == MIN_BIN)
            {
                std::vector<double> means = tempHist.getMean();
                std::vector<double> stdDev = tempHist.getStdDev();
                csv1.newRow() << group << std::to_string(i%2 + 1) << means[0] << means[1] << stdDev[0] << stdDev[1];
            }

            if (i%2 == 0)
                groupModel.push_back(tempHist);
            else
            {
                for (int k = 0; k < histMethods.size(); ++k)
                {
                    double distance = 
                        tempHist.calcDistance(groupModel[j - MIN_BIN], histMethods[k]);
                    csv2.newRow() << group << j << distance << methodNames[k].c_str();
                }
            }
        }

        if (i%2 == 1)
        {
            groupModel.clear();
            group++;
        }
 
        path.str("");
        path.clear();
    }

    csv1.writeToFile("playground/tomas/analysis/csv/meanstd_diff.csv", false);
    csv2.writeToFile("playground/tomas/analysis/csv/distance_diff.csv", false);
}