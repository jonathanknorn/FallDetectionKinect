#include<iostream>
#include <math.h>

#include "floor_detector.h"


using namespace std;
using namespace cv;

FloorDetector::FloorDetector() {
	for(int i = 0; i < 256; ++i){ 
		red.push_back(sin((i-50)*0.05 + 0) * 127 + 128);
		green.push_back(sin((i-50)*0.05 + 2) * 127 + 128);
		blue.push_back(sin((i-50)*0.05 + 4) * 127 + 128);
		dist.push_back(17.7269745-(31.89021096/log(i/272.7793312))); //Distance from depth value		
		normal.push_back((dist[i]/1.925012647)+0.5);
	}
}

cv::Mat FloorDetector::color(cv::Mat m)
{
	Mat colored = m.clone();
	uchar* mPixel;
	uchar* cPixel;
	double depth;
	for (int i = 0; i < m.rows; ++i)
	{
		mPixel = m.ptr<uchar>(i);
		cPixel = colored.ptr<uchar>(i);
		for (int j = 0; j < m.cols; ++j)
		{
			depth = *mPixel; 
			*cPixel++ = blue[depth]; 
			*cPixel++ = green[depth];
			*cPixel++ = red[depth];
			mPixel += 3;
		}
	}
	return colored;
}

cv::Mat FloorDetector::normalize(cv::Mat m)
{
	Mat n = m.clone();
	uchar* mPixel;
	uchar* nPixel;
	double depth;
	for (int i = 0; i < m.rows; ++i)
	{
		mPixel = m.ptr<uchar>(i);
		nPixel = n.ptr<uchar>(i);
		for (int j = 0; j < m.cols*3; ++j)
		{
			depth = *mPixel; 
			*nPixel++ = normal[depth]; 
			++mPixel;
		}
	}
	return n;
}

cv::Mat FloorDetector::calc_v_disparity(cv::Mat m){
	Mat disparity = Mat::zeros(m.size().height, 500, CV_8UC1);
	uchar* dispPointerOrigin = disparity.ptr<uchar>(0);
	double depth;
	for (int i = 0; i < m.rows; ++i) //For each row
	{
		uchar* dispPointer = disparity.ptr<uchar>(i);
		uchar* pixel = m.ptr<uchar>(i);  // Pointer to first pixel in row
		for (int j = 0; j < m.cols; ++j) //For each col
		{
			depth = *pixel; 
			int disp = static_cast<int>(4350/dist[depth]);
			
			dispPointer = dispPointerOrigin + //origin
										i*disparity.size().width + //rows
										10*disp; //disparity for that row
			if(disp < 50 && *dispPointer < 255)
			{
				for (int n = 0; n < 10; ++n)
				{ 
					*dispPointer += 1;
					++dispPointer;
				}
			}
			pixel+= 3;
		}
	}
	return disparity;
}

cv::Mat FloorDetector::canny(cv::Mat m){
	Mat c;
	Canny(m, c, 50, 300, 3);
	return c;
}

std::vector<std::pair<cv::Point,cv::Point>> FloorDetector::get_lines(cv::Mat m){
	vector<pair<Point,Point>> lines2;
	 vector<Vec2f> lines;

	 HoughLines(m, lines, 1, CV_PI/180, 100, 0, 0);

  for( size_t i = 0; i < lines.size(); i++ )
  {
     float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000*(-b));
		pt1.y = cvRound(y0 + 1000*(a));
		pt2.x = cvRound(x0 - 1000*(-b));
		pt2.y = cvRound(y0 - 1000*(a));
		lines2.push_back({pt1,pt2});
	}
	return lines2;
}	  
