#include<iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace std;
/* Matria */
void paintRectangles(cv::Mat &img, std::map<int, cv::Rect> &bboxes) {
	std::map<int, cv::Rect>::iterator it, it_end = bboxes.end();
	for (it = bboxes.begin(); it != it_end; it++) {
		cv::rectangle(img, it->second, cv::Scalar(0, 0, 255), 2);
	}
}

/* Materia */
void getBlobs(cv::Mat labels, std::map<int, cv::Rect> &bboxes) {
	int r = labels.rows, c = labels.cols;
	int label, x, y;
	bboxes.clear();
	for (int j = 0; j < r; ++j)
		for (int i = 0; i < c; ++i) {
			label = labels.at<int>(j, i);
			if (label > 0) {
				if (bboxes.count(label) == 0) { //New label
					cv::Rect r(i, j, 1, 1);
					bboxes[label] = r;
				}
				else { //Update rect
					cv::Rect &r = bboxes[label];
					x = r.x + r.width - 1;
					y = r.y + r.height - 1;
					if (i < r.x) r.x = i;
					if (i > x) x = i;
					if (j < r.y) r.y = j;
					if (j > y) y = j;
					r.width = x - r.x + 1;
					r.height = y - r.y + 1;
				}
			}
		}
}


int main(int argc, char *argv[]) {
	/* Capturo Camara */
	cv::VideoCapture vid;
	vid.open(0);

	if (!vid.isOpened()) {
		cerr << "Error opening input." << endl;
		return 1;
	}

	cv::Mat img, bg, fg, labels;
	cv::Ptr<cv::BackgroundSubtractorMOG2> mog;
	int historyMOG = 20; //Length of the history
	int nmixtures = 3; //Number of Gaussians Mixtures
	double backgroundRatio = 0.5; //Threshold of data that should be accounted for by the background (Parámetro T)
	double learningRate = 0.005; //Learning Rate
	double varThreshold = 16;


	bool bShadowDetection = false;
	mog = cv::createBackgroundSubtractorMOG2(historyMOG, varThreshold, bShadowDetection);
	mog->setNMixtures(nmixtures);
	mog->setBackgroundRatio(backgroundRatio);

	
	bool first = true;
	int i = 0, c;

	while (1) {
		vid >> img;
		i++;
		if (i > 10) {
			if (first) {
				first = false;
				img.copyTo(bg);
				mog->apply(bg, fg, learningRate);
			}
			else {
				if (i < 20) //Reinforce with initial background
					mog->apply(bg, fg, learningRate);
				else
					mog->apply(img, fg, learningRate);
				cv::erode(fg, fg, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));

				//imshow("Video", img);
				//imshow("Background", bg);
				imshow("Foreground",255-fg);


			}
		}
		if ((c = cv::waitKey(10)) != -1)
			break;
	}


	vid.release();

	cv::Mat gs(1, 256, CV_8UC3), YCrCb;
	for (int i = 0; i < 256; i++) {
		gs.data[3 * i] = gs.data[3 * i + 1] = gs.data[3 * i + 2] = i;
	}
	cv::cvtColor(gs, YCrCb, cv::COLOR_BGR2YCrCb);

	for (int i = 0; i < 256; i++) {
		cout << i << ": Y=" << (int)YCrCb.data[3 * i] << "; Cr=" << (int)YCrCb.data[3 * i + 1]
			<< "; Cb=" << (int)YCrCb.data[3 * i + 2] << ";" << endl;
	}

	return 0;
}



