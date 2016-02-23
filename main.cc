#include "utilities.h"
#include "floor_detector.h"
#include "logger.h"
#include "foreground_extractor.h"


//Simple test program, just shows a window with a normalized and colored depth frame and prints the fps.


int main()
{
    cv::VideoCapture cap(0);
    cv::Mat src;
    FloorDetector f;
    ForegroundExtractor fe;
    for(int i = 1; true; ++i){

        cap >> src;

        cv::resize(src, src, cv::Size(src.size().width/2, src.size().height/2)); //Scale down image to improve performance
        cv::Mat src2 = src.clone();

        f.normalize(src2);                                       //Normalize the depth in the frame
        f.color(src2, false);                                       //Normalize the depth in the frame
        f.color(src, false);                                       //Normalize the depth in the frame

        Utilities::show_mat("original frame", src);
        Utilities::show_mat("normalized", src2);

        if(cv::waitKey(30) >= 0) break;

    }
    return 0;
}
