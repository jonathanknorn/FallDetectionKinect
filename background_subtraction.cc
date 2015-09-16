#include "floor_detector.h"
#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
#include <math.h>       /* sin */

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>

using namespace cv;
using namespace std;
//int main(int argc, char *argv[])
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

    const int nmixtures =5;		//Set up background subtractor
    const bool bShadowDetection = false;
    const int history = 200;
    BackgroundSubtractorMOG2 bg (history,nmixtures,bShadowDetection);
	bg.set("backgroundRatio",0.7);

    namedWindow("Frame"); //Initialize frames
	namedWindow("Background");
	namedWindow("Masked");
	namedWindow("Double dilated");

    vector<vector<Point>> contours;

    VideoCapture cap(0); //Initialize camera

    for(int ff = 0;true;++ff) //Main loop, f is for periodical updates, e.g. for every 15th frame: if(f %15 == 0)
    {
        cap >> frame; //Save current frame and reduce it's size to 1/4th
        resize(frame, frame, Size(frame.size().width/2, frame.size().height/2) );
        f.normalize(frame);

		if(ff < 50){
		    bg.operator ()(frame,fore,0.02); //Add frame to background subtractor to get foreground mask
		    cout << "Adding background" << endl;
		    bg.getBackgroundImage(back);		//Get background frame for visualisation
		    f.normalize(back);
            imshow("Background",back);      //Background
        } else {
            if (ff % 100 == 0){
                bg.operator ()(frame,fore,0.02); //Add frame to background subtractor to get foreground mask
            } else {
                bg.operator ()(frame,fore,0.000); //Get mask, without
            }

            bg.getBackgroundImage(back);		                                //Get background frame for visualisation
            f.color(frame);

            Mat ffore = fore.clone();
            cv::erode(ffore,ffore,cv::Mat());                                   //Erode and dilate twice to eliminate noise, should be possible to do once with better settings
            cv::erode(ffore,ffore,cv::Mat());
            cv::dilate(ffore,ffore,cv::Mat());
            cv::dilate(ffore,ffore,cv::Mat());
            masked = Mat::zeros( frame.rows, frame.cols, CV_8UC3 );             //Create empty frame for mask
            frame.copyTo(masked, ffore);                                        //Apply mask to original frame and copy to masked

            findContours(ffore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE); //Find contours
            for(auto &v : contours){                                            //Mark larger objects with rectangles in original frame
                cout << v.size() << endl;
                if(v.size()>150){
                    Rect brect = boundingRect(Mat(v).reshape(2));
                    rectangle(frame, brect.tl(), brect.br(), Scalar(0,0,255),2,CV_AA);
                }
            }
            cout << endl << endl;
            f.color(back);                  //Color background for better visualization
            imshow("Frame",frame);          //Original frame
            imshow("Original mask",fore);   //Original mask
            imshow("Background",back);      //Background
            imshow("Masked",masked);        //Masked frame
        }
		if(waitKey(30) >= 0) break;
	}
    return 0;
}
