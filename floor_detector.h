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
		cv::Mat calc_v_disparity(cv::Mat m);
		cv::Mat color(cv::Mat m);
		cv::Mat normalize(cv::Mat m);
		cv::Mat canny(cv::Mat m);
		std::vector<std::pair<cv::Point,cv::Point>> get_lines(cv::Mat m);
        bool angle_is_greater_than(cv::Point p1,cv::Point p2, float limit);
    
	private:
		std::vector<double> dist;
		std::vector<double> red;
		std::vector<double> green;
		std::vector<double> blue;
		std::vector<double> normal;
};
#endif
