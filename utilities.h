#ifndef UTILITIES_H
#define UTILITIES_H

#include<opencv2/opencv.hpp>
#include <sys/types.h>
#include <sys/stat.h>

class Utilities{
public:
    static void show_mat(std::string, cv::Mat &);
    static int distance(int d);
    static int reversed_normalization(int d);
    static int normalize(int d);
    static void save_frame(cv::Mat &, std::string);
    
    static const double TAN285;    //Tangens of the horizontal field of view (28.5 degrees)
    static const double TAN224;    //Tangens of the vertical field of view (22.4 degrees)
    static const double RATIO;  //Scale factor of bits: 500cm / 255
};
#endif
