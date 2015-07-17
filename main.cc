#include "visualizer.h"
#include "floor_detector.h"
#include <vector>

using namespace std;
using namespace cv;

int main()
{
    VideoCapture cap(0);
	Mat src, frame, m, c1, n, ca;;
	FloorDetector f;
	
	int frame_count = 6;
	for(int i = 0; true; ++i){
		cap >> src;
		resize(src, frame, Size(src.size().width/2, src.size().height/2) );
	
		if(i % frame_count == 0){
			n = f.normalize(frame);
			c1 = f.color(n);
			Visualizer::show_mat("Frame", c1);
		}
	
		m = f.calc_v_disparity(frame);
		ca = f.canny(m);
		vector<pair<Point,Point>> vec = f.get_lines(ca);
		cvtColor(ca, ca, CV_GRAY2RGB);
		for(auto it = vec.begin(); it != vec.end(); ++it){
			Point pt1 = (*it).first;
			Point pt2 = (*it).second;
			line(ca, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
		}
	
		Visualizer::show_mat("Disparity", ca);
	
		if(waitKey(30) >= 0) break;
		
	}
	return 0;
}
