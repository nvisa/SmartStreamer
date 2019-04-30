#ifndef STABILIZATIONALGORITHMELEMENT_H
#define STABILIZATIONALGORITHMELEMENT_H

#include "algorithm/basealgorithmelement.h"
#include "algorithm/basealgorithmcommon.h"

class StabilizationAlgorithmElement : public BaseAlgorithmElement
{
public:
	explicit StabilizationAlgorithmElement(QObject *parent = 0);

	int init();
	int reallocate();
	int processAlgo(const RawBuffer &buf);
	int baseAlgorithmProcess(const RawBuffer &buf);
	int release();
	struct StabilizationControl {
		int initialize;
		int sensitivity;
	};

protected:
	BaseAlgorithmCommon::BaseVariables v;
	StabilizationControl control;
};

#endif // STABILIZATIONALGORITHMELEMENT_H
