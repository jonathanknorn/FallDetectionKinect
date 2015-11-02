#include "floor_detector.h"
#include "foreground_extractor.h"
#include "logger.h"
#include "time.h"

using namespace cv;
using namespace std;

int main()
{
    Mat frame; 		//Original frame
    Mat fore; 		//Foreground mask
    Mat back;		//Background mask
    Mat resizeF;	//Resized original frame
    Mat masked;		//Original frame masked by foreground frame
    Mat hist;       //Histogram
    Mat hist_copy;  //Copy of histogram
    FloorDetector f;

    namedWindow("Background");

    vector<vector<Point>> contours;

    ForegroundExtractor fe;

    VideoCapture cap(0); //Initialize camera

    char buff[20];
    time_t now = time(NULL);
    strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));


    Logger l(buff);

    clock_t frame_start, start;
    start = clock();

    for(int i = 0; i < 50; ++i){ //Capture initial 50 frames to build up the background image
        cap >> frame;
        if(waitKey(30) >= 0) break;
        resize(frame, frame, Size(frame.size().width/2, frame.size().height/2) );
        f.normalize(frame);
		fe.extract_foreground(frame, fore, 0.02);
		fe.get_background_image(back);
        imshow("Background",back);              //Display background
    }

    namedWindow("Frame");                       //Initialize windows
	namedWindow("Original mask");
	namedWindow("Masked");
    namedWindow("Histogram - original");
    namedWindow("Histogram");

    for(int ff = 0; true; ++ff) //Main loop, ff is for updates
    {
        frame_start = clock();

        cap >> frame; //Save current frame and reduce it's size to 1/4th

        if(waitKey(30) >= 0) break;

        resize(frame, frame, Size(frame.size().width/2, frame.size().height/2) );

        hist = f.calc_v_histogram(frame);           //Map with a histogram for each line in the frame
        f.enhance(hist, 20);                        //Enhances the histogram map for easier line detection
        hist_copy = hist.clone();                   //Copy histogram to show it both with and without detected lines

        f.normalize(frame);

        if (ff % 100 == 0){                                 //Update background each 100 frames
            fe.extract_foreground(frame, fore, 0.02);
            fe.get_background_image(back);                  //Get background frame for visualisation
            imshow("Background",back);
        } else {                                            //Else just get foreground without updating background
            fe.extract_foreground(frame, fore, 0.0);
        }

        Mat ffore = fore.clone();
        fe.erode(ffore);
        fe.dilate(ffore);
        masked = Mat::zeros( frame.rows, frame.cols, CV_8UC3 );             //Create empty frame for mask
        frame.copyTo(masked, ffore);                                        //Apply mask to original frame and copy to masked



        vector<pair<Point,Point>> lines = f.get_lines(hist);        //Vector with detected lines from the histogram
        f.mark_lines(hist, lines);                                  //Draw lines in the histogram


        f.color(frame);

        findContours(ffore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE); //Find and draw contours
        drawContours(frame, contours, -1, Scalar(255,0,0));

        l.write_int(ff);
        int contour_count = 0;
        for(auto &v : contours){                //Mark larger objects with rectangles in original frame
            float sinx = 0.9135;                //Sinus of 90-the angle of the camera
            if(v.size()>150){                   //Objects with more than 150 points in their contour are detected as persons
                Rect brect = boundingRect(Mat(v).reshape(2));           //Bounding rect for contour
                int depth = fe.average_depth(masked, brect, 0.15);   //Depth is calculated on the top 15% of the detected body
                int y = f.floorline_at_depth(depth);                   //Theoretical y-value of the floor at the given depth
                rectangle(frame, brect.tl(), Point2f(brect.x+brect.width, y*sinx*sinx), Scalar(0,0,255),1,CV_AA); //Bounding rect with the bottom adjusted for the floor
                circle(frame, Point2f(brect.x+brect.width/2, brect.y+(y*sinx*sinx-brect.y)/2), 4, Scalar(0,0,255),-1);  //Center of the bounding rect

                double tan285 = 0.543;      //Tangens of the horizontal field of view (28.5)
                double tan224 = 0.412;      //Tangens of the vertical field of view (22.4)

                double bw = tan285 * static_cast<double>(depth)*1.96;   //Width of half the field of view at the given distance
                double bh = tan224 * static_cast<double>(depth)*1.96;   //Height of the...

                double wr = 2*bw*(brect.width+2)/frame.size().width;        //Object width = field width (in cm) * object pixel width / field pixel width
                double hr = sinx * 2*bh*(2+y-brect.y)/frame.size().height; //Height same as width, but adjusted for camera tilt

                cout << hr << " cm" << endl;        //Print object height

                char str[200];                      //Display height and width in frame
                sprintf(str,"W:%f",wr);
                putText(frame, str, Point2f(brect.x, brect.y), FONT_HERSHEY_PLAIN, 0.8,  Scalar(0,0,255,255));
                sprintf(str,"h:%f",hr);
                putText(frame, str, Point2f(brect.x+brect.width, brect.y+brect.height/2), FONT_HERSHEY_PLAIN, 0.8,  Scalar(0,0,255,255));

                if(contour_count != 0){
                    l.write_tab();
                }
                l.write_line(3, contour_count, wr, hr);
                ++contour_count;
            }
        }
        if(contour_count == 0){
            l.write_line(0);
        }

        imshow("Frame",frame);          //Original frame
        imshow("Original mask",fore);   //Original mask
        imshow("Masked",masked);        //Masked frame
        imshow("Histogram - original", hist_copy);
        imshow("Histogram", hist);      //Histlay frames

        cout << "Frame time: " << (clock() - frame_start)/1000 << "ms" << endl;
        cout << "Average fps: " <<  (ff*1000000.0)/(clock() - start) << endl << endl;

	}
    return 0;
}
