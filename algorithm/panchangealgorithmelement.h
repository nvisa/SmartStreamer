#ifndef PANCHANGEALGORITHMELEMENT_H
#define PANCHANGEALGORITHMELEMENT_H

#include "basealgorithmelement.h"
#include "proto/KardelenAPI.grpc.pb.h"
#include "proto/KardelenAPI.pb.h"
#include "proto/AlgorithmCommunication.grpc.pb.h"
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
	};

	enum API
	{
		ALGORITHMCOMM = 0,
		KARDELEN = 1,
	};

	int init() override;
	int processAlgo(const RawBuffer &buf);
	int release();
	int setPanChangeInfo(kaapi::ListOfLocationInformation *listOfLocInfo);
	int setPanChangeInfoFrom(const AlgorithmCommunication::ListOfLocationInformation &listOfLocInfo);


	virtual QString getTypeString();

	uchar meta[4096];
	int initROI;

protected:
	state stateOfProcess;

	QElapsedTimer timer;
	kaapi::ListOfLocationInformation *listOfLocationInformation;
	AlgorithmCommunication::ListOfLocationInformation *listOfLocationInformationFromAlgComm;
	API api;
};

#endif // PANCHANGEALGORITHMELEMENT_
