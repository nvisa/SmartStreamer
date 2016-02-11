#ifndef STABILIZATIONALGORITHMELEMENT_H
#define STABILIZATIONALGORITHMELEMENT_H

#include "algorithm/basealgorithmelement.h"

class StabilizationAlgorithmElement : public BaseAlgorithmElement
{
	Q_OBJECT
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
	int getSensitivity() { return control.sensitivity; }
	void setSensitivity(int s) { control.sensitivity = s; }
	int getPrivacy() { return v.privacy; }
	void setPrivacy(int p) { v.privacy = p; }
	int getStabilization() { return v.stabilization; }
	void setStabilization(int s) {v.stabilization = s; }
protected:
	int reloadJson(const QJsonObject &node);
	QJsonObject resaveJson(const QJsonObject &node);
	StabilizationControl control;
};

#endif // STABILIZATIONALGORITHMELEMENT_H
