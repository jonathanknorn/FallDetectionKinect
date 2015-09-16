#ifndef FLOOR_DETECTOR_H
#define FLOOR_DETECTOR_H

#include<opencv2/opencv.hpp>

class FloorDetector{
	public:
		FloorDetector();
		cv::Mat calc_v_histogram(cv::Mat &);
		void enhance(cv::Mat&, int);
		void color(cv::Mat &);
		void normalize(cv::Mat &);
		std::vector<std::pair<cv::Point,cv::Point>> get_lines(const cv::Mat &);
		void mark_lines(cv::Mat &, std::vector<std::pair<cv::Point,cv::Point>> &);
		void reduce(cv::Mat &);
        int m;
        float k;
	private:
		std::vector<double> dist;
		std::vector<double> inv_dist;
		std::vector<double> red;
		std::vector<double> green;
		std::vector<double> blue;
		std::vector<double> normal;
		std::vector<int> disp;
		std::vector<int> disp_norm;
};
#endif
