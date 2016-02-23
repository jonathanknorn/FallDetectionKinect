#ifndef FLOOR_DETECTOR_H
#define FLOOR_DETECTOR_H

#include<opencv2/opencv.hpp>
#include<vector>

class FloorDetector{
	public:
		FloorDetector();
		cv::Mat calc_v_histogram(cv::Mat &);
		void enhance(cv::Mat&, int);
        std::vector<int> floor_distances(cv::Mat &, int);
		void normalize(cv::Mat &);
		void color(cv::Mat &, bool);
		void coordinate(cv::Mat &);
        void set_floor(cv::Mat &);
		int floorline_at_depth(int);
        float m;
        float k;
        double sin_x;
        double cos_x;
        int h;
	private:
		std::vector<double> red;
		std::vector<double> green;
		std::vector<double> blue;
};
#endif
