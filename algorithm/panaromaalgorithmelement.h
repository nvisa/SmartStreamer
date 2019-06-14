#ifndef PANAROMAALGORITHMELEMENT_H
#define PANAROMAALGORITHMELEMENT_H

#include "basealgorithmcommon.h"
#include "basealgorithmelement.h"

class PanaromaAlgorithmElement : public BaseAlgorithmElement
{
public:
	explicit PanaromaAlgorithmElement(QObject *parent = 0);
	int init();
	int reallocate();
	int processAlgo(const RawBuffer &buf);
	int baseAlgorithmProcess(const RawBuffer &buf);
	int release();
	struct PanaromaControl {
		uchar meta[4096];
		int pan;
		int tilt;
		int initialize;
	};

protected:
	BaseAlgorithmCommon::BaseVariables v;
	PanaromaControl control;
};

#endif // PANAROMAALGORITHMELEMENT_H
