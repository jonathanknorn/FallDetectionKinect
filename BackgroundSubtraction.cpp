#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>

using namespace cv;

int main(int argc, char *argv[])
{
    Mat frame;
    Mat fore;
    Mat back;
    Mat resizeF;

    VideoCapture cap(0);
    
    //cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	//cap.set(CV_CAP_PROP_FRAME_HEIGHT, 488);
   
    
    const int nmixtures =7;
    const bool bShadowDetection = true;
    const int history = 1;

    BackgroundSubtractorMOG2 bg (history,nmixtures,bShadowDetection);
	bg.set("backgroundRatio",0.01);
    namedWindow("Frame");
   namedWindow("Fore");
   namedWindow("Background");

    for(int i = 0;true;++i)
    {
        cap >> frame;
        resize(frame, resizeF, Size(frame.size().width/4, frame.size().height/4) );
        //if(i%15 == 0){
			bg.operator ()(resizeF,fore, 0.01);
			bg.getBackgroundImage(back);
        
        imshow("Frame",resizeF);
        imshow("Fore",fore);
        imshow("Background",back);
        if(waitKey(30) >= 0) break;
    }
    return 0;
}
