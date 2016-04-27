#include "utilities.h"
#include "floor_detector.h"
#include "logger.h"
#include "foreground_extractor.h"
#include <chrono>



//Simple test program, just shows a window with a normalized and colored depth frame and prints the fps.


int main()
{
    std::chrono::high_resolution_clock::time_point start, frame_start;

    cv::VideoCapture cap(0);
    cv::Mat src;
    FloorDetector f;
    ForegroundExtractor fe;
    start = std::chrono::high_resolution_clock::now();
    for(int i = 1; true; ++i){
        frame_start = std::chrono::high_resolution_clock::now();
        
        cap >> src;
        if(cv::waitKey(30) >= 0) break;

        cv::resize(src, src, cv::Size(src.size().width/2, src.size().height/2)); //Scale down image to improve performance
//        cv::Mat src2 = src.clone();

//        f.normalize(src2);                                       //Normalize the depth in the frame
//        f.color(src2, false);                                       //Normalize the depth in the frame
//        f.color(src, false);                                       //Normalize the depth in the frame

        
        auto now = std::chrono::high_resolution_clock::now();
        
        auto diff = now - frame_start;
        Utilities::show_mat("original frame", src);
//        Utilities::show_mat("normalized", src2);
        std::cout << "Frame " << i << "\n";
        std::cout << "Frame time: " << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << "ms\n";
        std::cout << "Average fps: " <<  static_cast<float>(1000*i)/static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count()) << "\n";
    }
    return 0;
}
