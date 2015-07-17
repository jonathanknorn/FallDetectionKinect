#include "visualizer.h"
#include<iostream>
#include<opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

void Visualizer::show_mat(String s, Mat m) {
	imshow(s,m);
}

