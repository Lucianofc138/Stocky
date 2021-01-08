#ifndef PROC_MISC_
#define PROC_MISC_

#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/bgsegm.hpp>

#include <map>

namespace stky
{

    // ------------------------ SCAN FLOOR LIBRARY -------------------------------
    struct MatchInfo
    {
        std::vector<cv::KeyPoint> keypts_temp;
        std::vector<cv::KeyPoint> keypts_roi;
        std::vector<cv::DMatch> good_matches;
    };

    struct RelevantPoints
    {
        std::vector<cv::Point2f> src_pts;
        std::vector<cv::Point2f> dst_pts;
    };

    /** 
     * Busca instancias de un template, en la imagen de un piso de repisa
     * utiliza Feature Matching con SURF
     * @param templateImg imagen del template a usar
     * @param floorImg imagen del piso de la repisa
     * @param boxes vector con los rect en donde se encontraron instancias
     */
    void scanFeaturesSlidingWindow(cv::Mat templateImg, cv::Mat floorImg, std::vector<cv::Rect> &boxes);

    /** 
     * Compara una imagen con un template, y decidi si coinciden o no
     * @param tempImg imagen del template a usar
     * @param boxImage imagen del sector de la repisa a comparar
     * @param numGoodMatch minima cantidad de matches
     * @param distThreshold distancia admitida
     * @return retorna verdadero o falso
     */
    bool doTheyMatch(cv::Mat tempImg, cv::Mat boxImage, 
                     int numGoodMatch, double distThreshold);
    // ------------------------ DRAW RESOURCES -------------------------------

    /** 
     * Dibuja los rectangulos con color en la imagen dada
     * @param image Imagen a utilizar
     * @param blobsMask Mascara en donde se debe dibujar
     * @param color Color de los rectangulos
     */
    void colorBlobsInImage(cv::Mat& image, cv::Mat& blobsMask, cv::Scalar color );

    // ------------------------ MOVEMENT MASK -----------------------------------

    struct BgSustractorInfo
    {
        int historyMOG = 130;
        int nmixtures = 5;
        double backgroundRatio = 0.07;
        double learningRate = 0.005;
        double varThreshold = 25;
        bool bShadowDetection = true;
        cv::Ptr<cv::BackgroundSubtractorMOG2> mog;
    };

    class RelevantFrames
    {
        public:
            /** 
             * Constructor de RelevantFrames
             * @param bg_ inicializar el background
             */
            RelevantFrames( cv::Mat bg_ = cv::Mat() );

            /** 
             * Constructor del BackgroundSubtractorMOG2
             */
            void initMog();

            /** 
             * Calcula la mascara del Foreground a traves del uso 
             * de BackgroundSubtractorMOG2
             */
            void calcFg();

            /** 
             * Constructor de frame
             * @param frame_ frame a setear
             */
            void setFrame(cv::Mat frame_);

            /** 
             * @return Matriz de mascara de foreground
             */
            cv::Mat getFg();

        private:
            cv::Mat frame;
            cv::Mat bg;
            cv::Mat fg;
            int frameNumber;
            BgSustractorInfo bgSustractor;

            void getMovementMask();
            
    };

}

#endif