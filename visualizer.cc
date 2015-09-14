#include "visualizer.h"

using namespace std;
using namespace cv;

void Visualizer::show_mat(String s, Mat m) {
	imshow(s,m);
}

