#include "floor_detector.h"
#include "spinning_barrier.h"

#include <chrono>
#include <thread>

//Same as main, but uses threads to improve performance
//Known issues:
//*  imshow-windows freeze occationally,
//   proposed solution of adding waitkey() after each call just makes it worse


std::chrono::high_resolution_clock::time_point start, frame_start;

cv::VideoCapture cap(0);


cv::Mat src1, src2;
cv::Mat current_frame_copy;

cv::Mat *next_frame = &src1;
cv::Mat *current_frame = &src2;

cv::Size reduced;

FloorDetector f1,f2;

SpinningBarrier sb(3);

//Reads next frame
void load_image(){
    //Start timer
    start = std::chrono::high_resolution_clock::now();
    for (int i = 1;true;++i){
        frame_start = std::chrono::high_resolution_clock::now();
        
        cap >> *next_frame;
        cv::waitKey(30);
        
        sb.wait(); //------- First barrier-------
        
        std::swap(next_frame, current_frame);
        
        sb.wait();  //------- Second barrier-------
        
        auto now = std::chrono::high_resolution_clock::now();
        auto diff = now - frame_start;
        
        std::cout << "Frame " << i << "\n";
        std::cout << "Frame time: " << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << "ms\n";
        std::cout << "Average fps: " <<  static_cast<float>(1000*i)/static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count()) << "\n\n";
    }
}

//Normalizes, colors and displays current frame
void normalize(){
    for (int i = 1;true;++i){
        cv::resize(*current_frame, *current_frame, reduced);
        current_frame_copy = current_frame->clone();
        
        f1.normalize(*current_frame);
        f1.color(*current_frame, false);
        cv::imshow("Normalized frame",*current_frame);
        
        sb.wait(); //------- First barrier-------
        sb.wait(); //------- Second barrier-------
    }
}

//Colors and displays current frame
void color(){
    for (int i = 1;true;++i){
        
        sb.wait(); //------- First barrier-------
        
        //Run after first barrier to eliminate synchronization-conflicts with normalize_thread
        f2.color(current_frame_copy, false);
        cv::imshow("Original frame",current_frame_copy);
        
        sb.wait(); //------- Second barrier-------
    }
}

int main() {
    //Read first frame
    cap >> *current_frame;
    if(cv::waitKey(30) >= 0){}
    
    //Calculate size of reduced frame
    reduced = cv::Size(current_frame->size().width/2, current_frame->size().height/2);
    
    //Initialize windows
    cv::namedWindow("Normalized frame");
    cv::namedWindow("Original frame");
    
    //Start threads
    std::thread load_thread(load_image);
    std::thread normalizing_thread(normalize);
    std::thread coloring_thread(color);
    
    load_thread.join();
    normalizing_thread.join();
    coloring_thread.join();
    
    return 0;
}
