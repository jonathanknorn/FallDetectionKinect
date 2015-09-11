#ifndef FLOOR_DETECTOR_H
#define FLOOR_DETECTOR_H

#include<opencv2/opencv.hpp>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
class FloorDetector{
	public:
		FloorDetector();
		cv::Mat calc_v_disparity(cv::Mat &);
		void enhance(cv::Mat&, int);
		void color(cv::Mat &);
		void normalize(cv::Mat &);
		std::vector<std::pair<cv::Point,cv::Point>> get_lines(const cv::Mat &);
        bool angle_is_greater_than(const cv::Point &,const cv::Point &, float);
        int m;
        float k;
	private:
		std::vector<double> dist;
		std::vector<double> red;
		std::vector<double> green;
		std::vector<double> blue;
		std::vector<double> normal;
		std::vector<int> disp;
		std::vector<int> disp_norm;
};
#endif
