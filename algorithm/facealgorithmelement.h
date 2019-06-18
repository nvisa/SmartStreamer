#ifndef FACEALGORITHMELEMENT_H
#define FACEALGORITHMELEMENT_H

#include "basealgorithmelement.h"
#include <QJsonObject>

class FaceAlgorithmElement : public BaseAlgorithmElement
{
public:
	explicit FaceAlgorithmElement(QObject *parent = 0);
	int init();
	int reallocate();
	int processAlgo(const RawBuffer &buf);
	int baseAlgorithmProcess(const RawBuffer &buf);
	int release();
	struct FaceControl {
		uchar meta[4096];
		int initialize;
		int camid;
		int numface;
		int frameRate;
		int counter;
	};

protected:
	FaceControl control;
	int reloadJson(const QJsonObject &node);
	QJsonObject resaveJson(const QJsonObject &node);
};

#endif // FACEALGORITHMELEMENT_H
