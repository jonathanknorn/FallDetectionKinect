#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
#include <math.h>       /* sin */

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>

using namespace cv;

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
    
    //cap.set(CV_CAP_PROP_FRAME_WIDTH, 640); //Might be necessary to force kinect to use IR-sensor
	//cap.set(CV_CAP_PROP_FRAME_HEIGHT, 488);
   
    
    const int nmixtures =7;		//Set up background subtractor
    const bool bShadowDetection = true;
    const int history = 1;
    BackgroundSubtractorMOG2 bg (history,nmixtures,bShadowDetection);
	bg.set("backgroundRatio",0.01);
	
	
    namedWindow("Frame"); //Initialize frames
	namedWindow("Fore");
	namedWindow("Background");
	namedWindow("Masked");
	
	
    VideoCapture cap(0); //Initialize camera

    for(int f = 0;true;++f) //Main loop, f is for periodical updates, e.g. for every 15th frame: if(f %15 == 0)
    {
        cap >> frame; //Save current frame and reduce it's size to 1/4th
        resize(frame, resizeF, Size(frame.size().width/4, frame.size().height/4) );
		
		bg.operator ()(resizeF,fore, 0.00); //Add frame to background subtractor to get foreground mask
		bg.getBackgroundImage(back);		//Get background frame for visualisation

		
		/**
		minMaxLoc(frame, &minVal, &maxVal); 
		std::cout << "Min: " << minVal << "\nMax: " << maxVal<<"\n";
		**/
		

		double depth;
		for (int i = 0; i < resizeF.rows; ++i) //For each row
		{
			uchar* pixel = resizeF.ptr<uchar>(i);  // Pointer to first pixel in row
			for (int j = 0; j < resizeF.cols; ++j) //For each col
			{
				depth = *pixel;  	//Get first color in pixel (All colors are the same since kinect gives a greyscale depth output
				if(i == resizeF.rows/2 && j == resizeF.cols/2){
					std::cout << "Mid:" << depth << "\n"; //Cout mid point depth
				}
						
				*pixel++ = blue[depth]; //Get pre calculated color for each pixel in frame
				*pixel++ = green[depth];
				*pixel++ = red[depth];
            
				
			}
		}
		
		masked = Mat::zeros( resizeF.rows, resizeF.cols, CV_8UC3 ); //Create black frame for mask
		resizeF.copyTo(masked, fore); //Apply foreground mask to original frame and copy to masked
    
		imshow("Frame",resizeF); //Present frames
		imshow("Fore",fore);
		imshow("Background",back);	
		imshow("Masked",masked);	
		
		if(waitKey(30) >= 0) break;
		
	}
    return 0;
}
