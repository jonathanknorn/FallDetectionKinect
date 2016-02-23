#include "floor_detector.h"
#include "utilities.h"

using namespace std;
using namespace cv;

FloorDetector::FloorDetector() {

    //RGB values for coloring the depth image
	for(int i = 0; i < 256; ++i){
		red.push_back(sin((i-50)*0.05 + 0) * 127 + 128);
		green.push_back(sin((i-50)*0.05 + 2) * 127 + 128);
		blue.push_back(sin((i-50)*0.05 + 4) * 127 + 128);
	}
}

//Returns the theoretical y-value of the floor at the given depth.
int FloorDetector::floorline_at_depth(int d){
    int floor = static_cast<int>(static_cast<float>(Utilities::reversed_normalization(d))*k) + m;
    return floor;
}

//Normalizes the depth values
void FloorDetector::normalize(cv::Mat &mat){
    uchar* mPixel;
    double depth;
    for (int i = 0; i < mat.rows; ++i){
        mPixel = mat.ptr<uchar>(i);
        for (int j = 0; j < mat.cols; ++j){
            depth = Utilities::normalize(*mPixel);
            *mPixel++ = depth;
            *mPixel++ = depth;
            *mPixel++ = depth;
        }
    }
}

//Colors the mat, diferent depths are given different colors
//if color_floor is true, the floor plane is colored white
void FloorDetector::color(cv::Mat &mat, bool color_floor){
    uchar* mPixel;
    double depth;
    for (int i = 0; i < mat.rows; ++i)
    {
        mPixel = mat.ptr<uchar>(i);
        float floor = (static_cast<float>(i) - m)/k;
        for (int j = 0; j < mat.cols; ++j)
        {
            depth = *mPixel;
            if(color_floor && abs(Utilities::reversed_normalization(static_cast<int>(depth)) - floor) < 1){
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

//Same as normalize, with the difference that the depth values are transformed to take the cameras angle in account
void FloorDetector::coordinate(cv::Mat &mat)
{
    uchar* mPixel;
    double depth;
    for (int i = 0; i < mat.rows; ++i)
    {
        mPixel = mat.ptr<uchar>(i);
        for (int j = 0; j < mat.cols; ++j)
        {
            depth = *mPixel;
            double z = Utilities::distance(depth);
            double bh = Utilities::TAN224 * static_cast<double>(z);
            double y = -2*bh*(i-static_cast<double>(mat.rows/2))/mat.rows;
            int ry = static_cast<int>(cos_x * y - sin_x * z);
            int rz = static_cast<int>(sin_x * y + cos_x * z);
            if(abs(ry - h) <= 1){
                *mPixel++ = 255;
                *mPixel++ = 255;
                *mPixel++ = 255;
            } else {
                uchar value = static_cast<uchar>(rz/2);
                *mPixel++ = blue[value];
                *mPixel++ = green[value];
                *mPixel++ = red[value];
            }
        }

    }
}

//Returns a vertical histogram (one histogram for each row) for the given frame
cv::Mat FloorDetector::calc_v_histogram(cv::Mat &mat){
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

//Enhances the disparity map for better line detection
void FloorDetector::enhance(cv::Mat &mat, int limit){
    vector<bool> v(256,false);
    for (int i = mat.rows-1; i >= 0 ; --i)
	{
		uchar* ptrOrig = mat.ptr<uchar>(i);
		bool first = true;
		for (int j = mat.cols; j >= 0; --j)
		{
            uchar* pixel = ptrOrig + static_cast<uchar>(j);
			if(first && *pixel > limit){
                if(v[j]){
                    *pixel = 0;
                } else {
                    *pixel = 255;
                    v[j] = true;
                }
                first = false;
			} else {
                *pixel = 0;
			}
		}
	}
}

vector<int> FloorDetector::floor_distances(cv::Mat &mat, int limit){
    vector<int> floor;
    for (int i = 0; i < mat.rows; ++i)
    {
        uchar* ptrOrig = mat.ptr<uchar>(i);
        for (int j = mat.cols; j >= 0; --j)
        {
            uchar* pixel = ptrOrig + static_cast<uchar>(j);
            if(*pixel > limit){
                floor.push_back(j);
                break;
            }
        }
    }
    return floor;
}

//Line detection based on the HoughLines-algorithm
void FloorDetector::set_floor(cv::Mat &mat){
    vector<Vec2f> lines;
    HoughLines(mat, lines, 1, CV_PI/180, 8, 0, 0);

    cvtColor(mat, mat, CV_GRAY2RGB);

    pair<Point,Point> floor_line;
    float min_cut = 0;
    float max_cut = mat.rows;

    for(auto it = lines.begin(); it != lines.end(); ++it){                                      //Iterate through the detected lines to find the one representing the floor
        float rho = (*it)[0];
        float theta = (*it)[1];

        Point pt1, pt2;

        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));

        float k_val = -a/b;
        float m_val = rho/b;

        float y_cut = mat.cols * k_val + m_val; //point where the line cuts the right side of the histogram
        float x_cut = (mat.rows-m_val)/k_val;
        if(y_cut > min_cut && y_cut < max_cut && x_cut > 0 && x_cut < mat.cols){// && m > -hist.rows){                     //The line with the lowest histarity(longest distance from the camera)
            floor_line = make_pair(pt1,pt2);                                                                       //at the bottom of the frame and positive k-value is the floor.
            m = m_val;
            k = k_val;
            min_cut = y_cut;
        }
        line(mat, pt1, pt2, Scalar(0,255,0), 1, CV_AA);       //Mark all detected lines green

    }

    double y1 = 6*mat.rows/8;
    double y2 = 7*mat.rows/8;
    double x1 = (y1 - m) / k;
    double x2 = (y2 - m) / k;

    double tz1 = static_cast<double>(Utilities::distance(x1));
    double ty1 = -2*Utilities::TAN224*tz1*(y1-static_cast<double>(mat.rows/2))/mat.rows;

    double tz2 = static_cast<double>(Utilities::distance(x2));
    double ty2 = -2*Utilities::TAN224*tz2*(y2-static_cast<double>(mat.rows/2))/mat.rows;

    double ang = atan((ty2-ty1)/(tz2-tz1));

    cos_x = cos(ang);
    sin_x = sin(ang);
    h = static_cast<int>(cos_x * ty1 - sin_x * tz1);


    line(mat, floor_line.first, floor_line.second, Scalar(255,0,0), 1, CV_AA);        //Mark the detected floor line blue
}
