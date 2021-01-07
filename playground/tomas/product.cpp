#include <histUtils/histUtils.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>

#include <CSVWriter.h>

const int NUM_SAMPLES = 8;

const std::string PRODUCT = "frosties";

int main() {

    CSVWriter csv;
    std::stringstream photo;
    std::vector<cv::Mat> photos;

    for (int i = 1; i <= NUM_SAMPLES; ++i)
    {
        photo << "samples/producto/cereales/" << PRODUCT << std::to_string(i) << ".png";
        std::cout << photo.str() << std::endl;
        cv::Mat current = cv::imread(photo.str(), cv::IMREAD_COLOR);
        photos.push_back(current);
        photo.str("");
        photo.clear();
    }

    std::stringstream model;
    model << "samples/producto/cereales/" << PRODUCT << "_m.png";
    cv::Mat frostieModel = cv::imread(model.str(), cv::IMREAD_COLOR);
    cv::Mat M_yCrCb, M_crCb;
    cv::cvtColor(frostieModel, M_yCrCb, cv::COLOR_BGR2YCrCb);
    std::vector<cv::Mat> M_channels;
    cv::split(M_yCrCb, M_channels);
    M_channels.erase(M_channels.begin());
    cv::merge(M_channels, M_crCb);

    Hist frostiesModelHisto(M_crCb);

    csv.newRow() << "Mean 0" << "Mean 1" << "StdDev 0" << "StdDev 1" << "Distance";

    for (auto image : photos)
    {
        cv::Mat yCrCb, crCb;
        cv::cvtColor(image, yCrCb, cv::COLOR_BGR2YCrCb);
        std::vector<cv::Mat> channels;
        cv::split(yCrCb, channels);
        channels.erase(channels.begin());
        cv::merge(channels, crCb);

        Hist curr_histo(crCb);

        std::vector<double> mean = curr_histo.getMean();
        std::vector<double> stdDev = curr_histo.getStdDev();
        double distance = curr_histo.calcDistance(frostiesModelHisto);

        csv.newRow() << mean[0] << mean[1] << stdDev[0] << stdDev[1] << distance;
    }
    csv.writeToFile("playground/tomas/analysis/csv/frosties.csv", false);
}