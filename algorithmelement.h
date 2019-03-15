#ifndef ALGORITHMELEMENT_H
#define ALGORITHMELEMENT_H

#include "lmm/baselmmelement.h"
#include "algorithmmanager.h"

#include <iostream>
#include <vector>
using namespace std;

#include <sstream>
#include <iomanip>

class AlgorithmElement : public BaseLmmElement
{
	Q_OBJECT
public:
	AlgorithmElement(QObject *parent = 0);

	enum Algorithm {
		MOTION,
		STABILIZATION,
		TRACKING,
		PANAROMA,
		FACE_DETECTION,
		NONE,
	};

	enum TrackMode {
		AUTO,
		SEMI_AUTO,
		MANUAL,
	};

	struct MotionAlg {
		int sensitivity;
		bool classification_;
		bool alarmFlag;
		int classification;
	};

	struct Stabilization {
		int sensitivity;
		int dummy;
	};

	struct TrackedObjects {
		float objWidth;
		float objHeight;
		float objPointX;
		float objPointY;
	};

	struct Tracking {
		float trackScore;
		int trackDuration;
		QMap<int,TrackMode> enabledModes;
		QMap<int, TrackedObjects> objects;
		float objWidth;
		float objHeight;
		float objPointX;
		float objPointY;
		int dummy;
	};

	struct Panaroma {
		int dummy;
	};

	struct FaceDetection {
		int dummy;
		bool isTileOn;
		int xTile;
		int yTile;
		int mode; // 0:rectangle, 1:privacy, else:imagecroplist
		bool isAlignmentOn;
	};

	struct AlgorithmHandler {
		MotionAlg motionA;
		Stabilization stabilA;
		Tracking trackA;
		Panaroma panaromA;
		FaceDetection faceA;
		Algorithm currentActiveAlg;
		int initialize;
		uchar meta[4096];
		bool systemParameters;
	};

	AlgorithmManager::AlgorithmHandler algHandler;
	AlgorithmManager::configurationUnit confUnit;
	AlgorithmManager::PTZinformation ptzInfo;

	bool setConfigurationElement(AlgorithmManager::AlgorithmHandler algHandler);
	bool setCurrentActiveAlgorithm(Algorithm alg);
	AlgorithmManager::PTZinformation forwardPTZaction(uchar meta[]);
	void updateAlgorithmParameters(AlgorithmHandler algHand, Algorithm alg);
	void updateAlgorithmParametersFromManager(AlgorithmManager::AlgorithmHandler algHand, Algorithm alg);
	int bufsize;
	// BaseLmmElement interface

	int init();
	int clean();
	int enableAlg(bool on);

protected:
	int reinit();
	int processBuffer(const RawBuffer &buf);
	int processAlgorithm(const RawBuffer &buf);
	int counter;

};

#endif // ALGORITHMELEMENT_H
