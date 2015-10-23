#include "foreground_extractor.h"

using namespace std;
using namespace cv;

ForegroundExtractor::ForegroundExtractor() {
  	bg.set("nmixtures", 5);
  	bg.set("detectShadows", false);
  	bg.set("history", 200);
  	bg.set("backgroundRatio",0.7);
}

void ForegroundExtractor::extract_foreground(const cv::Mat& frame, cv::Mat& mask, double learning_rate){ //Extracts the foreground
    bg.operator()(frame, mask, learning_rate);
}

void ForegroundExtractor::get_background_image(cv::Mat& mat){ //Returns the background image from the background extractor
    bg.getBackgroundImage(mat);
}

void ForegroundExtractor::erode(cv::Mat& mat){ //Erodes the mask
    cv::erode(mat, mat, cv::Mat());
}

void ForegroundExtractor::dilate(cv::Mat& mat){ //Dilates the mask
    cv::erode(mat, mat, cv::Mat());
}


int ForegroundExtractor::average_depth(cv::Mat &mat, cv::Rect &rect, float r){     //Returns a std::pair with the min and max depths of the top r*rect.height lines within rect in the given mat. Basically the min and max depth values of the top part of the rect.
    vector<int> v(256,0);
    uchar* mPixel;
	for (int i = rect.y; i < rect.y + static_cast<int>(static_cast<double>(rect.height)*r); ++i)
	{
		mPixel = mat.ptr<uchar>(i);
		mPixel += rect.x*3;
		for (int j = 0; j < rect.width; ++j)
		{
            ++v[*mPixel];
			mPixel+=3;
		}
	}
    int max = -1;
    int max_val = 0;
    for(int i = 1; i < 255; ++i){
//        cout << "I: " << i << ":" << v[i] << ": " << max_val << endl;
        if (v[i] > max_val) {
            max = i;
            max_val = v[i];
        }
    }
    return max;
}
