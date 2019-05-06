#ifndef MOTIONALGORITHMELEMENT_H
#define MOTIONALGORITHMELEMENT_H

#include "basealgorithmcommon.h"
#include "basealgorithmelement.h"

class MotionAlgorithmElement: public BaseAlgorithmElement
{
public:
	MotionAlgorithmElement(QObject *parent = 0);
	int init();
	int processAlgo(const RawBuffer &buf);
	int release();
	struct MotionControl {
		uchar meta[4096];
		float *panTiltZoomRead;
		bool alarmFlag;
		int initialize;
		int sensitivity;
		bool classification;
	};

protected:
	BaseAlgorithmCommon::BaseVariables v;
	MotionControl control;
};

#endif // MOTIONALGORITHMELEMENT_H
