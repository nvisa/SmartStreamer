#ifndef ALGORITHMELEMENT_H
#define ALGORITHMELEMENT_H

#include "lmm/baselmmelement.h"
#include "commoninformationinterface.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include <iostream>
#include <vector>
using namespace std;

#include <sstream>
#include <iomanip>
class AlgorithmManager;
class AlgorithmElement : public BaseLmmElement
{
	Q_OBJECT
public:
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
		TrackMode trackMode;
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

	struct parameters {
		int rgb;
		int ill;
		int debug;
		int shadow;
		int record;
		int privacy;
		int stabilization;
	};

	struct deviceProperties {
		QString cameraIp;
		int width;
		int height;
		float fps;
		int frameSize;
	};

	struct configurationUnit {
		parameters param;
		deviceProperties devProp;
	};

	struct AlgorithmElementHandler {
		configurationUnit confUnit;
		deviceProperties devProp;
		bool systemParameters;
		Algorithm currentActiveAlg = NONE;
		MotionAlg motionA;
		Stabilization stabilA;
		Tracking trackA;
		Panaroma panaromA;
		FaceDetection faceA;
		int initialize;
		uchar meta[4096];
	};

	AlgorithmElementHandler algHandlerEl;
	AlgorithmElement(QObject *parent = 0);
	PTZinformation ptzInfo;
	PTZinformation forwardPTZaction(uchar meta[]);
	bool setConfigurationElement(AlgorithmElement::AlgorithmElementHandler algHandler,AlgorithmElement::Algorithm alg);
	bool setCurrentActiveAlgorithm(Algorithm alg);
	void updateAlgorithmParameters(AlgorithmElementHandler algHand, Algorithm alg);
	void setAlgorithmManager(AlgorithmManager *algMan);
	int bufsize;
	int init();
	int clean();
	int enableAlg(bool on);
	bool isPassThru;
protected:
	int reinit();
	int processBuffer(const RawBuffer &buf);
	int processAlgorithm(const RawBuffer &buf);
	int counter;
	int skipFrameRate();
	AlgorithmManager *ptManager;
};

#endif // ALGORITHMELEMENT_H
