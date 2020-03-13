#ifndef ALGORITHMPARAMETERS
#define ALGORITHMPARAMETERS

#include <iostream>
using namespace std;

struct FaceDetection
{
	string detectionModel;
	string allignmentModel;
	bool tiling;
	bool includeFullFrame;
	int xTile;
	int yTile;
	string displayMode;
	string rectangleColor;
	bool allignment;
	bool saveFaces;
	string saveOptions;
	string saveDir;
	int allignmentErrorThreshold;
	float scoreThreshold;
	float sizePercentageUpperLimitX;
	float sizePercentageUpperLimitY;
	float sizePercentageLowerLimitX;
	float sizePercentageLowerLimitY;
	bool trackFaces;
	int bestFaceNum;
	int enlargePercentage;
	int saveFrameCount;
	int eraseFaceThresh;
	int validFaceThresh;
	float roiRatioX1;
	float roiRatioY1;
	float roiRatioX2;
	float roiRatioY2;
	int blurThreshold;
	int skipFrame;
};

struct ObjectDetection
{
	string detectionModel;
	bool tiling;
	bool includeFullFrame;
	int xTile;
	int yTile;
	string displayMode;
	string humanColor;
	string vehicleColor;
	float scoreThreshold;
	float sizePercentageUpperLimitX;
	float sizePercentageUpperLimitY;
	float sizePercentageLowerLimitX;
	float sizePercentageLowerLimitY;
	int skipFrame;
};


#endif // ALGORITHMPARAMETERS
