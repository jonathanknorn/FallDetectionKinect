#include "visualizer.h"
#include "floor_detector.h"

using namespace std;
using namespace cv;

//Simple test program, just shows a window with a normalized and colored depth frame and prints the fps.


int main()
{
    VideoCapture cap(0);
    Mat src;
    FloorDetector f;

    clock_t frame_start, start;
    start = clock();

    for(int i = 1; true; ++i){
        frame_start = clock();

        cap >> src;

//        resize(src, src, Size(src.size().width/2, src.size().height/2)); //Scale down image to improve performance

        f.normalize(src);                                       //Normalize the depth in the frame
        f.color(src);                                           //Color it for better visualization

        Visualizer::show_mat("Frame", src);


        cout << "Frame time: " << (clock() - frame_start)/1000 << "ms" << endl;
        cout << "Average fps: " <<  (i*1000000.0)/(clock() - start) << endl << endl;

        if(waitKey(30) >= 0) break;
    }
    return 0;
}
