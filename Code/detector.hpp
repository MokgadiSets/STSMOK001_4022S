 #include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/core.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <map>

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

const int noise = 0;
const int unclassified = -1;

class DBPoint {
public:
    double x, y;
    int  cluster;
    bool visited;
    int index ;
    double distance(DBPoint  Q) {
        return sqrt((x - Q.x)*(x-Q.x)+(y-Q.y)*(y-Q.y));
    }
};

class modified {
public:
	vector<DBPoint> neighborhood, wholeset;
};





vector<DBPoint> RangeQuery(vector<DBPoint>  database, DBPoint Q, int minDistance){
	vector<DBPoint> neighborsL;
	neighborsL.push_back(Q);
	for (int c = 0; c < database.size(); c++) {
		if (Q.distance(database[c]) <= minDistance) {
			neighborsL.push_back(database[c]);
		}
	}

return neighborsL;
}

modified expandCluster(vector<DBPoint> neighbors,vector<DBPoint> set ,int counter, int minDistance, int minPts) {
	modified output;
	vector<DBPoint> modifiedPoints = set;
	vector<DBPoint> modifiedNeighbors = neighbors;
	for (int e = 0; e < neighbors.size(); e++) {
		if (!neighbors[e].visited) {
			modifiedPoints[neighbors[e].index].visited = true;
			modifiedNeighbors[e].visited = true;
			vector<DBPoint> neighbors2 = RangeQuery(set, modifiedNeighbors[e], minDistance);
			if (neighbors2.size() >=  minPts) {
				//for (int i = 0; i < neighbors2.size(); i++) {
				//	if (!(std::find(neighbors.begin(),neighbors.end(),neighbors2[i])!= neighbors.end())) {
				//		modifiedNeighbors.push_back(neighbors[1]);
				//	}
				//}
				modifiedNeighbors.insert(modifiedNeighbors.end(),neighbors2.begin(),neighbors2.end());
			}
		}

		if (neighbors[e].cluster < 1) {
			modifiedPoints[neighbors[e].index].cluster = counter;
			modifiedNeighbors[e].cluster = counter;
		}
	}
	output.neighborhood = modifiedNeighbors;
	output.wholeset = modifiedPoints;
	return output;
}



vector<int> DBSCAN(vector<Point2f> points, int minDistance, int minPts  ) {


	vector<DBPoint> inputPoints;
	int counter = 0;
	int size = points.size();
	cout << "size = " << size << endl;
	for (int a = 0; a < size; a++){
		DBPoint objectPoint;
		objectPoint.x = points[a].y;
		objectPoint.y = points[a].x;
		objectPoint.visited = false ;
		objectPoint.index = a;
		objectPoint.cluster = unclassified;

		inputPoints.push_back(objectPoint);
	}

  	for (int b = 0; b < size; b++) {
		if (!inputPoints[b].visited) {
			inputPoints[b].visited = true;
			 vector<DBPoint> neighbors = RangeQuery(inputPoints, inputPoints[b], minDistance);
			if (neighbors.size() < minPts) {
				inputPoints[b].cluster = noise;
			}

			else {

				counter++;
				inputPoints[b].cluster = counter;
				modified output = expandCluster(neighbors, inputPoints, counter, minDistance, minPts);
				neighbors = output.neighborhood;
				inputPoints = output.wholeset;
			}
		}
	}



	vector<int> ptsC;

	for (int d = 0; d < size; d++) {
		ptsC.push_back(inputPoints[d].cluster);
	}

	return ptsC;
}


