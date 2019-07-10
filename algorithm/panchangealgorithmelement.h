#ifndef PANCHANGEALGORITHMELEMENT_H
#define PANCHANGEALGORITHMELEMENT_H

#include "basealgorithmelement.h"
#include "proto/KardelenAPI.grpc.pb.h"
#include "proto/KardelenAPI.pb.h"
#include "proto/AlgorithmCommunication.grpc.pb.h"
#include <QElapsedTimer>

#include "vector"
#include <QMap>
#include <QPair>
class PanChangeAlgorithmElement : public BaseAlgorithmElement
{
	Q_OBJECT
public:
	PanChangeAlgorithmElement(QObject *parent = 0);

	enum state
	{
		panIsGoingToLocation = 0,
		waitForStabilView = 1,
		algorithmIsPerforming = 2,
		algorithmIsCompletedMoveNextPoint = 3,
		waitForGivenInterval = 4,
	};

	enum API
	{
		ALGORITHMCOMM = 0,
		KARDELEN = 1,
	};

	int init() override;
	int processAlgo(const RawBuffer &buf);
	int release();
	int setPanChangeInfo(const kaapi::ListOfLocationInformation &listOfLocInfo);
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
	int locationIndex;
	std::vector<int> numberOfTurnAtGivenIndex;
	int saveLocations();
	int loadLocations();
};

#endif // PANCHANGEALGORITHMELEMENT_
