#ifndef MOTIONALGORITHMELEMENT_H
#define MOTIONALGORITHMELEMENT_H

#include "basealgorithmelement.h"
#include "proto/AlgorithmCommunication.grpc.pb.h"
#include "proto/OrionCommunication.grpc.pb.h"

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

	virtual QString getTypeString();

	struct MotionControl {
		uchar meta[4096];
		float *panTiltZoomRead;
		bool alarmFlag;
		int initialize;
		int sensitivity;
		bool classification;
	};

	int saveROI(AlgorithmCommunication::TRoi troi);
	AlgorithmCommunication::MotionParameters *loadROI();

#if HAVE_TK1
	int saveROI(const OrionCommunication::TRoi *troi);
	OrionCommunication::TRoi *loadROIOrion();
#endif

protected:
	int reloadJson(const QJsonObject &node);

	MotionControl control;
};

#endif // MOTIONALGORITHMELEMENT_H
