#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/core.hpp"
#include "dbscan.hpp"

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

class detected {
	Rect rectangle;
	double width;
	double height;
	double distance;
};

const Mat LMatrix = (Mat_<float>(3,3) << 782.7234, 0, 312.5103, 0, 781.4573, 200.7911, 0, 0, 1);
const Mat RMatrix = (Mat_<float>(3,3) << 776.7421, 0, 315.7450, 0, 775.8838, 229.3147, 0, 0, 1);
const vector<float> distCoeffL = {0.0885, -0.4207, -0.0117, 0.0041, 1.7684};
const vector<float> distCoeffR = {-0.0196, 1.0863, -0.0042, 0.0018, -5.8288};

int main(int argc, const char * argv[])
{
	//initailize cameras
	printf("time 1= %ld\n",time(NULL));
        VideoCapture capL("outputL.avi");
        if (!capL.isOpened()) {
                cerr<<"Error: Unable to open the left camera" << endl;
                return 0;
        }
		capL.set(CV_CAP_PROP_POS_FRAMES, 2250-1);
        VideoCapture capR("outputR.avi");
        if (!capR.isOpened()) {
                cerr<<"Error: Unable to open the right camera" << endl;
                return 0;
        }
		capR.set(CV_CAP_PROP_POS_FRAMES, 2250-1);
	//left and right image matrices and grabbing
        Mat frameLDist;
	Mat frameRDist;
        cout<< "Start grabbing, press a key on live window to terminate" << endl;
        while(1) {
                capL >> frameLDist;
		cout << time(NULL)<< endl;
		capR >> frameRDist;
		cout << time(NULL)<< endl;
                 if (!(frameLDist.empty()) && !(frameRDist.empty())) {
                     
		

		//Keypoint detection using SURF
		//Mat frameLGray, frameRGray;
		//cvtColor(frameLDist, frameLGray,CV_BGR2GRAY);
		//cvtColor(frameRDist, frameRGray, CV_BGR2GRAY);
		//Ptr<SURF> detector = SURF::create(100,7,5,false,true);
		Mat frameL, frameR;
		undistort(frameLDist, frameL, LMatrix, distCoeffL);
		undistort(frameRDist, frameR, RMatrix, distCoeffR);
		Ptr<FastFeatureDetector> detector = FastFeatureDetector::create(10,true);
		vector<KeyPoint> keypointsL, keypointsR;
		cout << keypointsL.size() << endl;

		detector -> detect(frameL, keypointsL);
		detector -> detect(frameR, keypointsR);

		Mat img_keypointsL; Mat img_keypointsR;

		drawKeypoints(frameL, keypointsL, img_keypointsL, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
		drawKeypoints(frameR, keypointsR, img_keypointsR, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

		imwrite("/home/pi/4022S/images/Left.jpg", img_keypointsL);
 		imwrite("/home/pi/4022S/images/Right.jpg", img_keypointsR);
		KeyPointsFilter filter = KeyPointsFilter();
		filter.retainBest(keypointsL,400);
		filter.retainBest(keypointsR,400);

		Mat descriptorsL, descriptorsR;
		Ptr<DescriptorExtractor> extractor = SURF::create();
		extractor -> compute(frameL,keypointsL,descriptorsL);
		extractor -> compute(frameR, keypointsR, descriptorsR);

		BFMatcher matcher(NORM_L2, true);
		vector<DMatch> matches;
		if (!descriptorsL.empty() && !descriptorsR.empty()){
			matcher.match(descriptorsL, descriptorsR,matches);
			Mat img3;
			drawMatches(frameL, keypointsL, frameR, keypointsR, matches, img3);
			imwrite("/home/pi/4022S/images/matches.jpg", img3);
			//waitKey();

			

			vector<Point2f>  matchpointsL, matchpointsR;
			int length = matches.size();

			for (int a = 0; a < length; a++){
				matchpointsL.push_back( keypointsL[matches[a].trainIdx].pt);
				matchpointsR.push_back( keypointsR[matches[a].queryIdx].pt);
			}

			vector<int> clusterIndices = DBSCAN(matchpointsL,90, 50);

			vector<Rect> rectangles;
			vector<double> aveDistances;
			vector<double> widths;
			vector<double> heights;
			for (int f = 1; f < *max_element(clusterIndices.begin(),clusterIndices.end()) + 1; f++) {
				vector<Point2f> cluster;
				vector<double> distances;
				double depth = 0.0;
				double disparity;
				for (int g = 0; g < length; g++) {
					if (clusterIndices[g] == f) {
						cluster.push_back(matchpointsL[g]);
						disparity = matchpointsL[g].x - matchpointsR[g].x;
						if  (disparity > 0) {
							distances.push_back((779*89)/disparity);
							depth = depth + ((779*89)/disparity);
						}
					}
				}
				if (cluster.size() > 0) {
					Rect border = boundingRect(cluster);
					double aveDistance = depth/distances.size();
					cout << depth << endl;
					//double height = border.height * ((0.0011 * aveDistance/10) + 0.0145);
					double height = border.height * ((0.0011 * 2300/10) + 0.0145);
					//double width = border.width * ((0.0011 * aveDistance/10) + 0.086);
					double width = border.width * ((0.0011 * 2300/10) + 0.086);
					rectangles.push_back(border);
					aveDistances.push_back(aveDistance);
					heights.push_back(height);
					widths.push_back(width);
				}
			}

			detected object;
			cout << "" << endl;
			cout << "Detected Object(s)" << endl;
			cout << "------------------------------" << endl;
			for ( int h=0; h < widths.size(); h++) {
				rectangle(frameL, rectangles[h], 0,1,8,0);
				cout << "object " << h+1 <<endl;
				cout << "rectangle: " << rectangles[h] << endl;
				cout << "distance: " << aveDistances[h]/1000 << "m" << endl;
				cout << "size: " << widths[h] << "x" << heights[h] << "cm" << endl;
				cout << "-----------------" <<endl;
			}
			
			imshow("rectangles", frameL);
			imwrite("/home/pi/4022S/images/result.jpeg",frameL);
			printf("time 2= %ld\n",time(NULL)); 
			cout << "--------------------------------" << endl;

		}
		else {
			cout << "" << endl;
			cout << "-----------------" << endl;
			cout << "Nothing interesting here!" << endl;
			imshow("rectangles",frameL);
			
		}}
		if (waitKey(0)==27) {
			break;
		}
	}
	cout<< "Closing the camera" << endl;
			capL.release();
			capR.release();
			destroyAllWindows();
	return 0;
}
