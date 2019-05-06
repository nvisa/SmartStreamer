#ifndef BASEALGORITHMELEMENT_H
#define BASEALGORITHMELEMENT_H

#include "lmm/baselmmelement.h"

class BaseAlgorithmElement : public BaseLmmElement
{
	Q_OBJECT
public:
	explicit BaseAlgorithmElement(QObject *parent = 0);

	enum AlgoState {
		INIT = 0x01,
		GPUINIT = 0x02,
		REINIT = 0x04,
		PROCESS = 0x08,
		REALLOCATE = 0x10,
		STOPALGO = 0x20,
		RELEASE = 0x40,
		UNKNOWN = 0xFF
	};

	virtual int init();
	virtual int gpuInit();
	virtual int reinit();

	virtual int processAlgo(const RawBuffer &buf);
	virtual int reallocate();
	virtual int stopAlgo();
	virtual int release();

	/* TODO: state management should be private */
	void setState(AlgoState state) { algoState = state; }
	int getState() { return algoState; }
protected:
	int processBuffer(const RawBuffer &buf);
	AlgoState algoState;

private:
	int baseAlgorithmProcess(const RawBuffer &buf);
};

#endif // BASEALGORITHMELEMENT_H
