#include "floor_detector.h"
#include "foreground_extractor.h"
#include "logger.h"
#include "time.h"
#include "utilities.h"

int main()
{
    //Original frame
    cv::Mat frame;

    //Foreground mask
    cv::Mat foreground_mask;

    //Background frame
    cv::Mat background;

    //Original frame with foreground mask applied
    cv::Mat masked_frame;

    //Histogram, used for floor detection
    cv::Mat histogram;

    //Initialization
    FloorDetector fd;
    ForegroundExtractor fe;

    //Stores contours found in image
    std::vector<std::vector<cv::Point>> contours;

    //Initialize camera
    cv::VideoCapture cap(0);
    
    //Initialize logger with current time
    char buff[32];
    time_t now = time(NULL);
    strftime(buff, 21, "%Y-%m-%d %H:%M:%S", localtime(&now));
    Logger l(buff);
    
    //Create folder for images
    char folder[40] = "images";
    strncat (folder, buff, 21);
    int i = Utilities::create_folder(folder);
    if(i != 0){
        std::cout << "Unable to create folder" << std::endl;
        return -1;
    }

    //Variables for fps-calculations
    clock_t start, frame_start;

    //Build the original background image from the 50 first frames
    for(int i = 0; i < 50; ++i){
        frame_start = clock();

        //Capture and resize frame, wait is needed for
        //frame to be fully read
        cap >> frame;

        if(cv::waitKey(10) >= 0) break;

        cv::resize(frame, frame, cv::Size(frame.size().width/2, frame.size().height/2) );

        //Preprocess frame (changes "out of bounds"-values
        //to work with the background subtraction)
        fe.preprocess(frame);

        //Add the current frame to the background
        //Frame makes 1/50th of the background (0.02)
        fe.add_foreground(frame, foreground_mask, 0.02);

        //Get the background image for visualization
        fe.get_background_image(background);


        //Display background
        Utilities::show_mat("Background",background);


        //Display original frame
        Utilities::show_mat("Frame",frame);

        std::cout << "Frame " << i << "/50!\n";
        std::cout << "Frame time: " << (clock() - frame_start)/1000 << "ms\n";
    }

    std::cout << "Done initializing!\n\n";


    start = clock();
    for(int frame_num = 0; true; ++frame_num) //Main loop, ff is for updates
    {
        frame_start = clock();
        //Capture and resize frame, wait is needed for
        //frame to be fully read
        cap >> frame;

        if(cv::waitKey(10) >= 0) break;

        cv::resize(frame, frame, cv::Size(frame.size().width/2, frame.size().height/2) );

        //Preprocess frame (changes "out of bounds"-values
        //to work with the background subtraction)
        fe.preprocess(frame);

        //Update background each 100 frames
        if (frame_num % 100 == 0){
            //Background extraction is same as in set up
            fe.add_foreground(frame, foreground_mask, 0.02);

            fe.get_background_image(background);

            //Calculate depth-histogram for each line in the background
            histogram = fd.calc_v_histogram(background);

             //Enhance the histogram map for easier line detection
            fd.enhance(histogram, 20);

            //Detect lines in the histogram and set
            //camera angle and height from detected floor line
            fd.set_floor(histogram);

            //Update foreground extractor with values of camera angle and height
            fe.setup_camera(fd.sin_x, fd.cos_x, fd.h);

            fd.normalize(background);
            fd.color(background, true);

        } else { //For the remaining 99/100 frames
            //only extract foreground mask without updating background
            fe.add_foreground(frame, foreground_mask, 0.0);
        }

        //remove shadows etc in the mask
        fe.enhance_mask(foreground_mask);
        fe.erode(foreground_mask);

        //initialize the masked frame as empty (black)
        masked_frame = cv::Mat::zeros( frame.rows, frame.cols, CV_8UC3 );

        //Apply foreground mask to original frame and copy to masked frame
        frame.copyTo(masked_frame, foreground_mask);

        //Normalize the masked frame for accurate distance-readings
        fd.normalize(masked_frame);

        //Normalize and color original frame,
        //(for visualization only)
        fd.normalize(frame);
        fd.color(frame, true);

        //Find contours in the mask and
        //mark them in the original frame
        cv::findContours(foreground_mask.clone(),contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE); //.clone() just needed for visualization, since findContours modifies the image
        cv::drawContours(frame, contours, -1, cv::Scalar(255,255,0));
        //Contours with less than 150 points in the frame
        //are considered noise or non-interesting
        for(auto &v : contours){
            if(v.size()>150){
                //Get the bounding rect for the contour
                cv::Rect brect = cv::boundingRect(cv::Mat(v).reshape(2));

                //Get the 3d-boundries of the object,
                //mark it in the original frame and
                //log the measurements in the log file.

                fe.boundries3d(masked_frame, frame, brect, l);
            }
        }


        //Save current frame and log detected objects for data collection
        Utilities::save_frame(frame, folder, std::to_string(static_cast<int>(clock() - start)/1000));

        l.write_line(1, (static_cast<double>(clock() - frame_start)/1000));

        //Print fps-data
        std::cout << "Frame time: " << (clock() - frame_start)/1000 << "ms\n";
        std::cout << "Average fps: " <<  (frame_num*1000000.0)/(clock() - start) << "\n";

        fd.color(masked_frame, false);

        //Show frames
        Utilities::show_mat("Frame",frame);
        Utilities::show_mat("Background",background);
        Utilities::show_mat("Foreground mask",foreground_mask);
        Utilities::show_mat("Masked frame",masked_frame);
        Utilities::show_mat("Histogram",histogram);


    }
    return 0;
}
