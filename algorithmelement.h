#ifndef ALGORITHMELEMENT_H
#define ALGORITHMELEMENT_H

#include "lmm/baselmmelement.h"
#include "algorithmmanager.h"

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
		bool classification;
		bool alarmFlag;
		int dummy;
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
	int bufsize;
	// BaseLmmElement interface
protected:
	int processBuffer(const RawBuffer &buf);
	int processAlgorithm(const RawBuffer &buf);

};

#endif // ALGORITHMELEMENT_H
