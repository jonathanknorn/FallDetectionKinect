#ifndef FOREGROUND_EXTRACTOR_H
#define FOREGROUND_EXTRACTOR_H

#include<opencv2/opencv.hpp>

class ForegroundExtractor{
	public:
		ForegroundExtractor();
        void extract_foreground(const cv::Mat&, cv::Mat&, double);
        void get_background_image(cv::Mat&);
        void erode(cv::Mat&);
        void dilate(cv::Mat&);
	private:
        cv::BackgroundSubtractorMOG2 bg;
};
#endif
