#include "processingMisc/processingMisc.hpp"

void paintRectangles(cv::Mat &img, std::map<int, cv::Rect> &bboxes, cv::Scalar color, int thickness = 3) {
	std::map<int, cv::Rect>::iterator it, it_end = bboxes.end();
	for (it = bboxes.begin(); it != it_end; it++) {
		cv::rectangle(img, it->second, color, thickness);
	}
}

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

void stky::colorBlobsInImage(cv::Mat& image, cv::Mat& blobsMask, cv::Scalar color){
    cv::Mat labels;
    cv::connectedComponents(blobsMask, labels, 8, CV_32S);
    std::map<int, cv::Rect> bboxes;
    getBlobs(labels, bboxes);
    paintRectangles(image, bboxes, color);

}