#include "foreground_extractor.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>

using namespace std;
using namespace cv;

ForegroundExtractor::ForegroundExtractor() {
  	bg.set("nmixtures", 5);
  	bg.set("detectShadows", false);
  	bg.set("history", 200);
  	bg.set("backgroundRatio",0.7);
}

void ForegroundExtractor::extract_foreground(const cv::Mat& frame, cv::Mat& mask, double learning_rate){
    bg.operator()(frame, mask, learning_rate);
}

void ForegroundExtractor::get_background_image(cv::Mat& mat){
    bg.getBackgroundImage(mat);
}

void ForegroundExtractor::erode(cv::Mat& mat){
    cv::erode(mat, mat, cv::Mat());
}

void ForegroundExtractor::dilate(cv::Mat& mat){
    cv::erode(mat, mat, cv::Mat());
}
