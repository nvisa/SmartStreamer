#ifndef STABILIZATIONALGORITHMELEMENT_H
#define STABILIZATIONALGORITHMELEMENT_H

#include "algorithm/basealgorithmelement.h"

class StabilizationAlgorithmElement : public BaseAlgorithmElement
{
public:
	explicit StabilizationAlgorithmElement(QObject *parent = 0);

	int init();
	int reallocate();
	int processAlgo(const RawBuffer &buf);
	int release();
	struct StabilizationControl {
		int initialize;
		int sensitivity;
	};
	virtual QString getTypeString();

	int ZoomLevelNo;
	float zoomvalues[1000*3];
	void zoom2degree_conversion(int zoomReadOut,float* HV_fovAngles);

protected:
	int reloadJson(const QJsonObject &node);
	StabilizationControl control;
};

#endif // STABILIZATIONALGORITHMELEMENT_H
