#ifndef MOTIONALGORITHMELEMENT_H
#define MOTIONALGORITHMELEMENT_H

#include "basealgorithmcommon.h"
#include "basealgorithmelement.h"

class MotionAlgorithmElement: public BaseAlgorithmElement
{
	Q_OBJECT
public:
	MotionAlgorithmElement(QObject *parent = 0);
	int init() override;
	int processAlgo(const RawBuffer &buf);
	int release();

	int getSensitivity();
	bool getClassification();
	void setSensitivity(int value);
	void setClassification(bool value);

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
