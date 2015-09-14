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

        vector<pair<Point,Point>> vec = f.get_lines(hist);      //Vector with detected lines in the histogram map
        cvtColor(hist, hist, CV_GRAY2RGB);
        cout << vec.size() << " lines detected" << endl;
        pair<Point,Point> floor_line;
        float max_cut = 0.0;

        for(auto it = vec.begin(); it != vec.end(); ++it){                      //Iterate through the detected lines to find the one representing the floor
            Point pt1 = (*it).first;
            Point pt2 = (*it).second;
            float k = -1.0;
            float m = 0.0;
            float delta_x = static_cast<float>(pt2.x - pt1.x);                  //Calculations of k and m for the lines (y=kx+m)
            float delta_y = static_cast<float>(pt2.y - pt1.y);
            if(delta_x != 0){
                k = delta_y/delta_x;

                m = static_cast<float>(pt1.y) - k * static_cast<float>(pt1.x);
                float cut = (hist.rows - m)/ k;
                if(cut > max_cut && k < 0){// && m > -hist.rows){                     //The line with the lowest histarity(longest distance from the camera)
                    floor_line = *it;                                           //at the bottom of the frame and positive k-value is the floor
                    f.m = static_cast<int>(m);
                    f.k = k;
                    max_cut = cut;
                    cout << "K: " << k << " M: "<<m << endl;
                }
                line(hist, pt1, pt2, Scalar(0,255,0), 1, CV_AA);       //Mark all detected lines green
            }
        }

        Point pt1 = floor_line.first;
        Point pt2 = floor_line.second;
        line(hist, pt1, pt2, Scalar(255,0,0), 1, CV_AA);        //Mark the detected floor line blue

        f.normalize(src);                                       //Normalize the depth in the frame
        f.color(src);                                           //Color it for better visualization



        Visualizer::show_mat("histarity", hist);                                        //histlay frames
        Visualizer::show_mat("Frame", src);
        Visualizer::show_mat("histarity - original", hist_copy);

        cout << "Frame time: " << (clock() - frame_start)/1000 << "ms" << endl;
        cout << "Average fps: " <<  (i*1000000.0)/(clock() - start) << endl << endl;

        if(waitKey(30) >= 0) break;
    }
    return 0;
}
