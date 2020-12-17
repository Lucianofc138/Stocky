#ifndef PERSP_UTILS
#define PERSP_UTILS

#include <opencv2/imgproc.hpp>
#include "opencv2/calib3d.hpp"

// #include "opencv2/imgcodecs.hpp"
// #include "opencv2/videoio.hpp"
// #include "opencv2/highgui.hpp"
// #include <opencv2/opencv.hpp>
// #include "opencv2/xfeatures2d.hpp"
// #include "opencv2/features2d.hpp"
// #include <iostream>

/**
 * Se le aplica un cambio de perspectiva a la srcImg (imagen), 
 * segun los puntos entregados en perspectiveMat, delimitada por dstSize
 * 
 * @param srcImg Imagen de entrada (matriz)
 * @param perspectiveMat Matriz de perspectiva
 * @param dstSize Tamaño de imagen de salida (matriz)
 * @return Imagen de salida con el cambio de perspectiva (matriz)
 */
cv::Mat getShelfFrontView(cv::Mat srcImg, cv::Mat perspectiveMat, cv::Size dstSize);

/**
 * Se obtiene la matriz de perspectiva necesaria para aplicarle a frame, 
 * segun los puntos de  entrada, y el tamaño solicitado
 * 
 * @param frame Imagen obtenida de Video (matriz)
 * @param srcPoints Vector de puntos de entrada
 * @param dstSize Tamaño de imagen de salida (matriz)
 * @return matriz para cambio de perspectiva
 */
cv::Mat getHomography(cv::Mat frame, std::vector < cv::Point2f > srcPoints, cv::Size dstSize);

#endif