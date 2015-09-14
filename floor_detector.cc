#include "floor_detector.h"

using namespace std;
using namespace cv;

FloorDetector::FloorDetector() {
	for(int i = 0; i < 256; ++i){
		red.push_back(sin((i-50)*0.05 + 0) * 127 + 128);                                        //RGB values for coloring the depth image
		green.push_back(sin((i-50)*0.05 + 2) * 127 + 128);
		blue.push_back(sin((i-50)*0.05 + 4) * 127 + 128);

        //Precalculations related to the normalization
        dist.push_back(18.47-(31.07/log(i/272.1)));                                             //Distance in cm for not normalized depth value
		inv_dist.push_back(272.1*exp(31.07/(18.47-i*1.953125)));//                              //Distance in cm for normalized depth value
		normal.push_back((dist[i]/1.953125)+0.5);                                               //Normalized depth(Distance in cm divided by 500/256

		//Start values for k and m,
		k = -1;
		m = 0;
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
            float floor_line = ((static_cast<float>(i) - m) / k) -0.0;                          //Calculates the theoretical value of the floor from the k- and m-values
            float floor = inv_dist[depth];                                                      //Converts the normalized depth to non-normalized (since the floor
                                                                                                //calculations are based on non normalized depth
            int diff = static_cast<int>(floor_line) - static_cast<int>(floor);                  //Difference between actual end theoretical value
            if(abs(diff) <= 1 && depth < 255){                                                  //If the difference is less than 1.5, the pixel is marked as on the floor
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

void FloorDetector::normalize(cv::Mat &mat)                                                     //Normalizes the depth values to a linear scale (pixel value * 1.953125 is distance in cm)
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

cv::Mat FloorDetector::calc_v_histogram(cv::Mat &mat){                                          //Returns a vertical histogram for the given frame
	Mat v_histogram = Mat::zeros(mat.size().height, 256, CV_8UC1);
	double depth;
	for (int i = 0; i < mat.rows; ++i)
	{
		uchar* histPointerOrigin = v_histogram.ptr<uchar>(i);
		uchar* pixel = mat.ptr<uchar>(i);
		for (int j = 0; j < mat.cols; ++j)
		{
			depth = *pixel;
			uchar* histPointer = histPointerOrigin + static_cast<uchar>(depth);
			if(depth < 250 && *histPointer < 255)
			{
				*histPointer += 1;
            }
			pixel+= 3;
		}
	}
	return v_histogram;
}

void FloorDetector::enhance(cv::Mat &mat, int limit){                                           //Enhances the disparity map for better line detection
	for (int i = 0; i < mat.rows; ++i)
	{
		uchar* ptrOrig = mat.ptr<uchar>(i);
		bool first = true;
		for (int j = mat.cols; j >= 0; --j)
		{
            uchar* pixel = ptrOrig + static_cast<uchar>(j);
			if(*pixel > limit && first){                                                        //the pixel on each line with the largest value (longest
                *pixel = 255;                                                                   //distance from camera) with a value larger than the
                first = false;                                                                  //limit is colored white, all others black. In order to
			} else {                                                                            //make the floor detection more accurate.
                *pixel = 0;
			}
		}
	}
}

std::vector<std::pair<cv::Point,cv::Point>> FloorDetector::get_lines(const cv::Mat &mat){       //Line detection based on the HoughLines-algorithm
    vector<Vec2f> lines;
    vector<pair<Point,Point>> point_lines;

	HoughLines(mat, lines, 1, CV_PI/180, 80, 0, 0);

    for(size_t i = 0; i < lines.size(); ++i){
        float rho = lines[i][0], theta = lines[i][1];                                           //Conversion to point-form
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000*(-b));
		pt1.y = cvRound(y0 + 1000*(a));
		pt2.x = cvRound(x0 - 1000*(-b));
		pt2.y = cvRound(y0 - 1000*(a));
		point_lines.push_back({pt1,pt2});
	}
	return point_lines;
}
