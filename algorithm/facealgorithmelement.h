#ifndef FACEALGORITHMELEMENT_H
#define FACEALGORITHMELEMENT_H

#include "basealgorithmelement.h"
#include <iostream>
#include "algorithmparameters.h"
#include <QJsonObject>

using namespace std;

class FaceAlgorithmElement : public BaseAlgorithmElement
{
	Q_OBJECT
public:
	explicit FaceAlgorithmElement(QObject *parent = 0);
	int init();
	int reallocate();
	int processAlgo(const RawBuffer &buf);
	int release();
	virtual QString getTypeString();

	struct FaceControl {
		uchar meta[4096];
		int initialize;
		int camId;
		int numface;
		int frameRate;
		int counter;
		int debug;
	};
	FaceDetection getFaceParams();
	void setFaceParams(const QJsonObject &node);

protected:
	FaceControl control;
	FaceDetection faceParams;
	int reloadJson(const QJsonObject &node);
};

#endif // FACEALGORITHMELEMENT_H
