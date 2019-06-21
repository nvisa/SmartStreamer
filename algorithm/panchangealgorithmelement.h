#ifndef PANCHANGEALGORITHMELEMENT_H
#define PANCHANGEALGORITHMELEMENT_H

#include "basealgorithmelement.h"
#include "proto/KardelenAPI.grpc.pb.h"
#include "proto/KardelenAPI.pb.h"
#include <QElapsedTimer>

class PanChangeAlgorithmElement : public BaseAlgorithmElement
{
	Q_OBJECT
public:
	PanChangeAlgorithmElement(QObject *parent = 0);

	enum state
	{
		panIsGoingToLocation = 0,
		algorithmIsPerforming = 1,
		algorithmIsCompletedMoveNextPoint = 2,
		waitForGivenInterval = 3,
		terminateAlgorithm = 4,
	};


	int init() override;
	int processAlgo(const RawBuffer &buf);
	int release();
	int setPanChangeInfo(kaapi::ListOfLocationInformation *listOfLocInfo);

	virtual QString getTypeString();

	uchar meta[4096];
	int initROI;
	bool secondTurn;

protected:
	state stateOfProcess;

	QElapsedTimer timer;
	kaapi::ListOfLocationInformation *listOfLocationInformation;
};

#endif // PANCHANGEALGORITHMELEMENT_H
