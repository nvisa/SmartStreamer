#ifndef FACEALGORITHMELEMENT_H
#define FACEALGORITHMELEMENT_H

#include "basealgorithmcommon.h"
#include "basealgorithmelement.h"

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
	BaseAlgorithmCommon::BaseVariables v;
	FaceControl control;
};

#endif // FACEALGORITHMELEMENT_H
