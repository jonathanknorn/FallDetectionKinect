#include "floor_detector.h"
#include "foreground_extractor.h"
#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
#include <math.h>       /* sin */

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>

using namespace cv;
using namespace std;

int main()
{

	vector<double> red;
	vector<double> green;
	vector<double> blue;

	for(int i = 0; i < 256; ++i){ //Pre calculate colors for depth-coloring
		red.push_back(sin((i-50)*0.05 + 0) * 127 + 128);
		green.push_back(sin((i-50)*0.05 + 2) * 127 + 128);
		blue.push_back(sin((i-50)*0.05 + 4) * 127 + 128);
	}

    Mat frame; 		//Original frame
    Mat fore; 		//Foreground mask
    Mat back;		//Background mask
    Mat resizeF;	//Resized original frame
    Mat masked;		//Original frame masked by foreground frame

    FloorDetector f;

    namedWindow("Background");

    vector<vector<Point>> contours;


    ForegroundExtractor fe;

    VideoCapture cap(0); //Initialize camera
    clock_t frame_start;
    for(int i = 0; i < 50; ++i){
        cap >> frame; //Save current frame and reduce it's size to 1/4th
        if(waitKey(30) >= 0) break;
        resize(frame, frame, Size(frame.size().width/2, frame.size().height/2) );
        f.normalize(frame);
		fe.extract_foreground(frame, fore, 0.02);
		fe.get_background_image(back);		//Get background frame for visualisation
        imshow("Background",back);      //Background
    }
    namedWindow("Frame"); //Initialize windows
	namedWindow("Original mask");
	namedWindow("Masked");

    for(int ff = 0;true;++ff) //Main loop, f is for periodical updates, e.g. for every 15th frame: if(f %15 == 0)
    {
        frame_start = clock();
        cap >> frame; //Save current frame and reduce it's size to 1/4th
        if(waitKey(30) >= 0) break;
        resize(frame, frame, Size(frame.size().width/2, frame.size().height/2) );
        f.normalize(frame);
        if (ff % 100 == 0){
            fe.extract_foreground(frame, fore, 0.02);
            fe.get_background_image(back);		//Get background frame for visualisation
            imshow("Background",back);      //Background
        } else {
            fe.extract_foreground(frame, fore, 0.0);
        }
        f.color(frame);
        Mat ffore = fore.clone();
        fe.erode(ffore);
        fe.dilate(ffore);
        masked = Mat::zeros( frame.rows, frame.cols, CV_8UC3 );             //Create empty frame for mask
        frame.copyTo(masked, ffore);                                        //Apply mask to original frame and copy to masked
        findContours(ffore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE); //Find contours
        for(auto &v : contours){                                            //Mark larger objects with rectangles in original frame
            if(v.size()>150){
                Rect brect = boundingRect(Mat(v).reshape(2));
                rectangle(frame, brect.tl(), brect.br(), Scalar(0,0,255),2,CV_AA);
            }
        }
        imshow("Frame",frame);          //Original frame
        imshow("Original mask",fore);   //Original mask
        imshow("Masked",masked);        //Masked frame

        cout << "Total frame: " << (clock() - frame_start)/1000 << "ms" << endl << endl;
	}
    return 0;
}
