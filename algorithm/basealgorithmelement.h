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

	struct BaseVariables {
		int rgb;
		int ill;
		int debug;
		int shadow;
		int record;
		int privacy;
		int stabilization;
	};

	virtual int init();
	virtual int gpuInit();
	virtual int reinit();

	virtual int processAlgo(const RawBuffer &buf);
	virtual int reallocate();
	virtual int stopAlgo();
	virtual int release();

	virtual QString getTypeString();
	int reloadJson();
	void setJsonAlgorithmIndex(int index);
	int getAlgorithmIndex() { return algIndex; }
	void restart();

	/* TODO: state management should be private */
	void setState(AlgoState state);
	int getState() { return algoState; }

	int savetoJson();
protected:
	virtual int reloadJson(const QJsonObject &node);
	virtual QJsonObject resaveJson(const QJsonObject &node);
	int processBuffer(const RawBuffer &buf);
	AlgoState algoState;
	BaseVariables v;

private:
	int baseAlgorithmProcess(const RawBuffer &buf);

	int algIndex;
	bool restarting;
};

#endif // BASEALGORITHMELEMENT_H
