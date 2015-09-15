#include "visualizer.h"
#include "floor_detector.h"

using namespace std;
using namespace cv;


int main()
{
    VideoCapture cap(0);
	Mat src, hist, hist_copy;
    FloorDetector f;

    clock_t frame_start, start;
    start = clock();
    for(int i = 1; true; ++i){
        frame_start = clock();

        cap >> src;

        resize(src, src, Size(src.size().width/2, src.size().height/2)); //Scale down image to improve performance

        hist = f.calc_v_histogram(src);         //Map with a histogram for each line in the frame
        f.enhance(hist, 20);                    //Enhances the histogram map for easier line detection
        hist_copy = hist.clone();

        vector<pair<Point,Point>> lines = f.get_lines(hist);      //Vector with detected lines in the histogram map
        cout << lines.size() << " lines detected" << endl;
        f.mark_lines(hist, lines);

        f.normalize(src);                                       //Normalize the depth in the frame
        f.color(src);                                           //Color it for better visualization

        Visualizer::show_mat("histogram", hist);                                        //histlay frames
        Visualizer::show_mat("Frame", src);
        Visualizer::show_mat("histogram - original", hist_copy);

        cout << "Frame time: " << (clock() - frame_start)/1000 << "ms" << endl;
        cout << "Average fps: " <<  (i*1000000.0)/(clock() - start) << endl << endl;

        if(waitKey(30) >= 0) break;
    }
    return 0;
}
