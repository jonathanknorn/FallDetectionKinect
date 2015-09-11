#include<iostream>
#include <math.h>

#include "floor_detector.h"


using namespace std;
using namespace cv;

FloorDetector::FloorDetector() {
	for(int i = 0; i < 256; ++i){
		red.push_back(sin((i-50)*0.05 + 0) * 127 + 128);                //RGB values for coloring the depth image
		green.push_back(sin((i-50)*0.05 + 2) * 127 + 128);
		blue.push_back(sin((i-50)*0.05 + 4) * 127 + 128);

        //Precalculations related to the normalization
        dist.push_back(17.7269745-(31.89021096/log(i/272.7793312)));    //Distance in cm from not normalized depth value
		normal.push_back((dist[i]/1.925012647)+0.5);                    //Normalized depth
		disp.push_back(static_cast<int>(4350/dist[i]));                 //Disparity for non-normalized depth values
		disp_norm.push_back(static_cast<int>(4350/(1.925012647*i)));    //Disparity for normalized depth values

		//Start values for k and m,
		k = 1;
		m = 480;
	}
}

void FloorDetector::color(cv::Mat &mat)
{
	uchar* mPixel;
	double depth;

	for (int i = 0; i < mat.rows; ++i)
	{
		mPixel = mat.ptr<uchar>(i);
		for (int j = 0; j < mat.cols; ++j)
		{
			depth = *mPixel;
            float floor_line = ((static_cast<float>(i) - m) / k) -0.0;      //Calculates the theoretical value of the floor from the k- and m-values
            float floor_dist = 4350.0/floor_line;                           //Converts the theoretical floor-value from disparity to centimeter

            float diff = floor_dist / (depth * 1.925012647);                //ratio between actual end theoretical value
            diff = 1.0 - diff;

            if(abs(diff) < 0.10){                                           //If the difference is less than 10%, the pixel is marked as on the floor
                *mPixel++ = 255;
                *mPixel++ = 255;
                *mPixel++ = 255;
			} else {
                *mPixel++ = blue[depth];
                *mPixel++ = green[depth];
                *mPixel++ = red[depth];
            }
		}
	}
}

void FloorDetector::normalize(cv::Mat &mat)                                 //Normalizes the depth values to a linear scale (pixel value * 1.925012647 is distance in cm)
{
	uchar* mPixel;
	double depth;
	for (int i = 0; i < mat.rows; ++i)
	{
		mPixel = mat.ptr<uchar>(i);
		for (int j = 0; j < mat.cols*3; ++j)
		{
		    depth = *mPixel;
			*mPixel++ = normal[depth];
		}

	}
}

cv::Mat FloorDetector::calc_v_disparity(cv::Mat &mat){                      //Returns a v-disparity map for the given frame
	Mat disparity = Mat::zeros(mat.size().height, 50, CV_8UC1);
	uchar* dispPointerOrigin = disparity.ptr<uchar>(0);
	double depth;
	for (int i = 0; i < mat.rows; ++i) //For each row
	{
		uchar* dispPointer = disparity.ptr<uchar>(i);
		uchar* pixel = mat.ptr<uchar>(i);  // Pointer to first pixel in row
		for (int j = 0; j < mat.cols; ++j) //For each col
		{
			depth = *pixel;
			int dispp = disp[depth];
			dispPointer = dispPointerOrigin + //origin
										i*disparity.size().width + //rows
										dispp; //disparity for that row
			if(depth != 255 && dispp < 50 && *dispPointer < 255)
			{
				*dispPointer++ += 1;
            }
			pixel+= 3;
		}
	}
	return disparity;
}

void FloorDetector::enhance(cv::Mat &mat, int limit){       //Enhances the disparity map for better line detection
	for (int i = 0; i < mat.rows; ++i)
	{
		uchar* pixel = mat.ptr<uchar>(i);
		bool first = true;
		for (int j = 0; j < mat.cols; ++j)
		{

			if(*pixel > limit && first){                    //the pixel with the smallest disparity (longest distance from camera)
                *pixel++ = 255;                             //with a disparity-value larger than the limit is colored white, all
                first = false;                              //others black.
			} else {
                *pixel++ = 0;
			}
		}
	}
}

std::vector<std::pair<cv::Point,cv::Point>> FloorDetector::get_lines(const cv::Mat &mat){ //Line detection based on the HoughLines-algorithm
	vector<pair<Point,Point>> lines2;
	 vector<Vec2f> lines;

	 HoughLines(mat, lines, 1, CV_PI/180, 80, 0, 0);

  for( size_t i = 0; i < lines.size(); i++ )
  {
     float rho = lines[i][0], theta = lines[i][1];      //Conversion to point-form
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

bool FloorDetector::angle_is_greater_than(const cv::Point &p1,const cv::Point &p2, float limit){  //Calculates if the given line has an angle greater than the limit
    float alpha ;
    float delta_x = static_cast<float>(p2.x -p1.x);
    float delta_y = static_cast<float>(p2.y - p1.y);

    bool draw = false;
    alpha = 0;
    float min =limit * M_PI/180.0;
    float max = M_PI_2 - min;

    if(delta_x != 0){
        alpha = atanf(delta_y/delta_x);
    }

    float fmodded = fmod(alpha,M_PI_2);

    if(   (fmodded> min && fmodded < max) || (-fmodded> min && -fmodded < max)){

        draw = true;
        return draw;

    }
    return draw;
}
