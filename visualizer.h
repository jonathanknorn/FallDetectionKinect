#ifndef VISUALIZER_H
#define VISUALIZER_H
#include<opencv2/opencv.hpp>
#include <string>

class Visualizer{
public:
	static void show_mat(std::string s, cv::Mat m);
};
#endif
