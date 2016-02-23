#ifndef FOREGROUND_EXTRACTOR_H
#define FOREGROUND_EXTRACTOR_H

#include<opencv2/opencv.hpp>
#include<vector>
#include "logger.h"
#include "utilities.h"

class ForegroundExtractor{
	public:
		ForegroundExtractor();
        void add_foreground(const cv::Mat&, cv::Mat&, double);
        void get_background_image(cv::Mat&);
        void preprocess(cv::Mat&);
        void erode(cv::Mat&);
        void enhance_mask(cv::Mat&);
        void boundries3d(cv::Mat&,cv::Mat&, cv::Rect&, Logger&);
        void setup_camera(double ,double, int);
    private:
        double sin_x;
        double cos_x;
        int h;
        cv::BackgroundSubtractorMOG2 bg;
        cv::Point invert_rotation(double, double, double, double, double);
};
#endif
