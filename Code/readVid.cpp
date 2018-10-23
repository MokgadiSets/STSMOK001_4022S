#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/core.hpp"
#include <wiringPi.h>


using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;




int main(int argc, const char * argv[])
{
	wiringPiSetup();
	pinMode(0, INPUT);
	pullUpDnControl(0,PUD_UP);
	pinMode(3, OUTPUT);
	digitalWrite(3,LOW);
	while(1) {
	if (digitalRead(0)==0){
		break;
		}
	}
	//initailize cameras
		digitalWrite(3,HIGH);
        VideoCapture capL(0);
        if (!capL.isOpened()) {
                cerr<<"Error: Unable to open the left camera" << endl;
                return 0;
        }

        VideoCapture capR(1);
        if (!capR.isOpened()) {
                cerr<<"Error: Unable to open the right camera" << endl;
                return 0;
        }

	//left and right image matrices and grabbing
        Mat frameLDist;
		Mat frameRDist;
        cout<< "Start grabbing, press a key on live window to terminate" << endl;
        VideoWriter outputL("outputL.avi", cv::VideoWriter::fourcc('M', 'J','P','G'),15, cv::Size(640,480));
        VideoWriter outputR("outputR.avi", cv::VideoWriter::fourcc('M', 'J','P','G'),15, cv::Size(640,480));
        while(1) {
                capL >> frameLDist;
				capR >> frameRDist;
	
                 if (frameLDist.empty()) {
                        cerr<< "Unable to grab from the left camera"<<endl;
                        break;
                }
                
			
				outputL.write(frameLDist);
				if (frameRDist.empty()) {
					cerr << "Unable to grab from right camera" << endl;
					break;
				}
				
			
				outputR.write(frameRDist);
				
				if (digitalRead(0)==1) {
					break;
				}
		}
		digitalWrite(3,LOW);
		capL.release();
		capR.release();
		outputL.release();
		outputR.release();
		destroyAllWindows();
		return 0;
	}
