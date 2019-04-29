#ifndef MOTIONALGORITHMELEMENT_H
#define MOTIONALGORITHMELEMENT_H

#include "basealgorithmcommon.h"
#include "basealgorithmelement.h"

class MotionAlgorithmPriv;
class MotionAlgorithmElement: public BaseAlgorithmElement
{
public:
	MotionAlgorithmElement(QObject *parent = 0);
	int init();
	int reallocate();
	int processAlgo(const RawBuffer &buf);
	int baseAlgorithmProcess(const RawBuffer &buf);
	struct MotionControl {
		uchar meta[4096];
		float *panTiltZoomRead;
		bool alarmFlag;
		int initialize;
		int sensitivity;
		bool classification;
	};

	void setState(BaseAlgorithmElement::AlgoState state);
protected:
	BaseAlgorithmCommon::BaseVariables v;
	MotionControl control;
};

#endif // MOTIONALGORITHMELEMENT_H
