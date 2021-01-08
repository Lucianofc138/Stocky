// #include <iostream>
// #include <vector>

// #include "opencv2/imgproc.hpp"
// #include "opencv2/imgcodecs.hpp"
// #include "opencv2/highgui.hpp"
// #include <opencv2/core/mat.hpp> 
// #include <opencv2/core/types.hpp> 


// using namespace cv;
// using namespace std;

// float* getChromaHistogram(cv::Mat BGRimg, const int bins);
// double BhattacharyyaDistance(float *h1, float *h2, int N);

// int main(int argc, char **argv)
// {

//     cv::Mat image = imread("samples/sample4_1.jpg", IMREAD_COLOR);
//     cv::Mat image2 = imread("samples/sample4_1_feik2.jpg", IMREAD_COLOR); 
//     // Check if image is loaded fine
//     if ( image.empty() ){
//         printf("Error opening image!");
//         return EXIT_FAILURE;
//     }
//     if ( image2.empty() ){
//         printf("Error opening image 2!");
//         return EXIT_FAILURE;
//     }
//     imshow("Original", image);
//     imshow("Feik", image2);

//     // --------------------------------------------------------------------------
//     // --------------------------------------------------------------------------

//     // std::vector<cv::Rect> columns, columns2;
//     // int columns_amount = 20;
//     // int x = 0, width = image.cols, height = image.rows;
//     // int col_wid = width/columns_amount;

//     // for(int i=0; i<columns_amount; i++){
//     //     cv::Rect new_col(x, 0, col_wid-1, height);
//     //     columns.push_back(new_col);
//     //     String win_tag = "";
//     //     win_tag.push_back((char)('0' + i));
//     //     imshow(win_tag, image2(new_col));
//     //     x += col_wid;
//     // }

//     // std::vector<float*> colsHistos1, colsHistos2;
//     // for(int i=0; i<columns2.size(); i++){
//     //     cv::Mat colImage = image(columns.at(i));
//     //     float* new_histo = getChromaHistogram(colImage, 16);
//     //     colsHistos1.push_back(new_histo);

//     //     cv::Mat colImage2 = image(columns2.at(i));
//     //     float* new_histo2 = getChromaHistogram(colImage2, 16);
//     //     colsHistos2.push_back(new_histo2);
//     // }

//     cv::Rect rect(630, 0, 100, image.rows);
//     cv::Mat roi1 = image(rect);
//     cv::Mat roi2 = image2(rect);

//     int bins = 16;
//     float* histo1 = getChromaHistogram(roi1,  bins);
//     float* histo2 = getChromaHistogram(roi2, bins);
//     float sum1 = 0.0, sum2 = 0.0;

//     double d_bat1 = BhattacharyyaDistance(histo1, histo2, bins*bins);
//     double d_bat2 = BhattacharyyaDistance(histo2, histo2, bins*bins);

//     imshow("sin equivocado", roi1);
//     imshow("con equivocado", roi2);

//     // --------------------------------------------------------------------------
//     // --------------------------------------------------------------------------

//     cv::waitKey(0);
//     return EXIT_SUCCESS;
// }

// float* getChromaHistogram(cv::Mat BGRimg, const int bins) {
//     cv::Mat HLSimg;
//     cv::cvtColor(BGRimg, HLSimg, cv::COLOR_BGR2HLS);

//     int i, j, idx, w = HLSimg.cols, h = HLSimg.rows;
//     int bsize = 256/bins, bb = bins*bins;


//     std::vector<Mat> channels;
//     cv::split(HLSimg, channels);

//     float *histo = new float[bb];
//     memset(histo, 0, bb*sizeof(float));

//     uchar channelHue, channelSat;
//     float valid_pixels = 0.0;
//     float histo_sum = 0;
//     for(int i=0; i<h; i++){
//         for(int j=0; j<w; j++){
//             channelHue = channels.at(0).at<unsigned int>(i, j);
//             channelSat = channels.at(2).at<unsigned int>(i, j);
//             if (true){ //condicion de Luma entre minimo y maximo ej: entre 30 y 220
//                 int index = (channelHue/bsize)*bins + (channelSat/bsize);
//                 histo[index] += 1.0;
//                 valid_pixels += 1.0;
//             }
            
//         }
//     }
//     for(int i=0; i<bb; i++){ //normalizacion
//         histo[i] /= valid_pixels;
//     }

//     return histo;
// }

// //The Bhattacharyya Distance, for normalized histograms
// double BhattacharyyaDistance(float *h1, float *h2, int N) {
//     double D = 0.0;
//     for(int i=0; i<N; i++)
//         D += sqrt(h1[i]*h2[i]);
//     return 1 - D < 0 ? 0.0 : sqrt(1 - D);
// }