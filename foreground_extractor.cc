#include "foreground_extractor.h"

using namespace std;
using namespace cv;


ForegroundExtractor::ForegroundExtractor() {
    //Setup values for the background subtractor
    bg.set("nmixtures", 5);
    bg.set("detectShadows", true);
    bg.set("history", 200);
    bg.set("backgroundRatio",0.7);
}

//Adds the current frame to the background subtractor
//learning_rate defines the weight of the added frame to be added to the background
void ForegroundExtractor::add_foreground(const cv::Mat& frame, cv::Mat& mask, double learning_rate){
    bg.operator()(frame, mask, learning_rate);
}

//Returns the background image from the background extractor
void ForegroundExtractor::get_background_image(cv::Mat& mat){
    bg.getBackgroundImage(mat);
}


//Changes value of "Out of bounds" pixels to the maximum distance (Background detections doesn't work in those pixels otherwise)
void ForegroundExtractor::preprocess(cv::Mat& mat){
    uchar* mPixel;
    for (int i = 0; i < mat.rows; ++i)
    {
        mPixel = mat.ptr<uchar>(i);
        for (int j = 0; j < mat.cols*3; ++j)
        {
            if(*mPixel < 1){
                *mPixel++ = 255;
            } else {
                mPixel++;
            }
        }
    }
}

//Erodes the mask, to remove edges from objects (which results in inacurate depth measurements)
void ForegroundExtractor::erode(cv::Mat& mat){
    cv::erode(mat, mat, cv::Mat());
}

//Sets all pixels marked as shadows to black, and detected objects to white in the mask
void ForegroundExtractor::enhance_mask(cv::Mat &mat){
    uchar* mPixel;
    for (int i = 0; i < mat.rows; ++i)
    {
        mPixel = mat.ptr<uchar>(i);
        for (int j = 0; j < mat.cols; ++j)
        {
            if(*mPixel == 127){
                *mPixel++ = 255;
            } else {
                *mPixel++ = 0;
            }
        }
    }
}

void ForegroundExtractor::boundries3d(cv::Mat &mat, cv::Mat &mat2, cv::Rect& rect, Logger& l){

    double near = -1, far = -1, top = -1, bottom = -1, left = -1, right = -1;
    uchar* mPixel;

    for (int i = 0; i < rect.height; ++i)
    {
        //set mPixel to the leftmost pixel in the rect
        mPixel = mat.ptr<uchar>(rect.y + i) + rect.x*3;

        for (int j = 0; j < rect.width; ++j) {
            if(*mPixel != 255 && *mPixel != 0){
                double z = (*mPixel)*Utilities::RATIO; //depth from camera plane in cm

                double bw = Utilities::TAN285 * z;   //Width of half the field of view at the given distance
                double bh = Utilities::TAN224 * z;   //Height of half the...

                double x = 2*bw*(rect.x + j-static_cast<double>(mat.cols/2))/mat.cols;       //Object width = field width (in cm) * object pixel width / field pixel width
                double y = -2*bh*((rect.y + i)-static_cast<double>(mat.rows/2))/mat.rows;   //Object height calculated the same way

                //transform coordinates by using rotation matrix on x and y-values
                double rx = x;
                double ry = cos_x * y - sin_x * z - h;
                double rz = sin_x * y + cos_x * z;

                //Check and set bounds
                if(top == -1 || ry > top){
                    top = ry;
                }
                if(bottom == -1 || ry < bottom){
                    bottom = ry;
                }
                if(far == -1 || rz > far){
                    far = rz;
                }
                if(near == -1 || rz < near){
                    near = rz;
                }
                if(right == -1 || rx > right){
                    right = rx;
                }
                if(left == -1 || rx < left){
                    left = rx;
                }
            }
            mPixel+=3;
        }
    }
    
    //Print and log bounds
    cout << "Bounds: " << near << " " << far << " " << left << " " << right << " " << bottom << " " << top << endl;
    l.write_doubles(6, near, far, left, right, bottom, top);

    
    //calculate coordinates in original image from bounds
    Point xyz = invert_rotation(left, top, near, static_cast<double>(mat.cols),static_cast<double>(mat.rows));
    Point Xyz = invert_rotation(right, top, near, static_cast<double>(mat.cols),static_cast<double>(mat.rows));
    Point xYz = invert_rotation(left, bottom, near, static_cast<double>(mat.cols),static_cast<double>(mat.rows));
    Point XYz = invert_rotation(right, bottom, near, static_cast<double>(mat.cols),static_cast<double>(mat.rows));
    
    Point xyZ = invert_rotation(left, top, far, static_cast<double>(mat.cols),static_cast<double>(mat.rows));
    Point XyZ = invert_rotation(right, top, far, static_cast<double>(mat.cols),static_cast<double>(mat.rows));
    Point xYZ = invert_rotation(left, bottom, far, static_cast<double>(mat.cols),static_cast<double>(mat.rows));
    Point XYZ = invert_rotation(right, bottom, far, static_cast<double>(mat.cols),static_cast<double>(mat.rows));

    //mark lines in mat2
    line(mat2,  xyz,  Xyz, Scalar(0,0,255));
    line(mat2,  xyz,  xYz, Scalar(0,0,255));
    line(mat2,  xyz,  xyZ, Scalar(0,0,255));
    line(mat2,  Xyz,  XYz, Scalar(0,0,255));
    line(mat2,  Xyz,  XyZ, Scalar(0,0,255));
    line(mat2,  xYz,  XYz, Scalar(0,0,255));
    line(mat2,  xYz,  xYZ, Scalar(0,0,255));
    line(mat2,  XYz,  XYZ, Scalar(0,0,255));
    line(mat2,  xyZ,  XyZ, Scalar(0,0,255));
    line(mat2,  xyZ,  xYZ, Scalar(0,0,255));
    line(mat2,  xYZ,  XYZ, Scalar(0,0,255));
    line(mat2,  XyZ,  XYZ, Scalar(0,0,255));
}

void ForegroundExtractor::setup_camera(double sinx, double cosx, int height){
    sin_x = sinx;
    cos_x = cosx;
    h = height;
}

//Inverts rotation of coordinates
Point ForegroundExtractor::invert_rotation(double x, double y, double z, double cols, double rows){

    double xx = x;
    double yy = cos_x*(y+h) + sin_x * z;
    double zz = -sin_x*(y+h) + cos_x * z;

    double bw = Utilities::TAN285 * zz;   //Width of half the field of view at the given distance
    double bh = Utilities::TAN224 * zz;   //Height of half the...

    double jPos = xx*cols/(2*bw)+cols/2;       //Object width = field width (in cm) * object pixel width / field pixel width
    double iPos = yy*rows/(-2*bh)+rows/2;

    return Point2f(jPos,iPos);
}
