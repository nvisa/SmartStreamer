#include "panchangealgorithmelement.h"
#include "algorithmfunctions.h"
#include "applicationinfo.h"
#include <ecl/ptzp/ptzphead.h>
#include <ecl/ptzp/ptzpdriver.h>

#include "lmm/debug.h"

PanChangeAlgorithmElement::PanChangeAlgorithmElement(QObject *parent)
	: BaseAlgorithmElement(parent)
{
	algoState = UNKNOWN;
	listOfLocationInformation = new kaapi::ListOfLocationInformation();
}

int PanChangeAlgorithmElement:: init()
{
	initROI = 1;
	stateOfProcess = panIsGoingToLocation;
	return BaseAlgorithmElement::init();
}

int PanChangeAlgorithmElement::processAlgo(const RawBuffer &buf)
{
	mInfo("Processing Algorithm %d", buf.constPars()->videoHeight);
	PtzpHead *headpt = ApplicationInfo::instance()->getPtzpDriver(0)->getHead(1);

	int width = buf.constPars()->videoWidth;
	int height = buf.constPars()->videoHeight;
	static int locationSize;
	static int locationIndex = 0;
	static long time;
	if (api == API::KARDELEN) {
		locationSize = listOfLocationInformation->locationinformation_size();
		time = listOfLocationInformation->intervalforcirculation();
	} else if (api == API::ALGORITHMCOMM) {
		locationSize = listOfLocationInformationFromAlgComm->locationinformation_size();
		time = listOfLocationInformationFromAlgComm->intervalforcirculation();
	}

	float panTiltRead[2];

	if (stateOfProcess == panIsGoingToLocation) {
		if (api == API::KARDELEN) {
			kaapi::LocationInformation myLocation = *(listOfLocationInformation->mutable_locationinformation(locationIndex));
			panTiltRead[0] = (float)myLocation.pan(); panTiltRead[1] = (float)myLocation.tilt();
			headpt->panTiltGoPos(panTiltRead[0],panTiltRead[1]);
			if (abs(headpt->getPanAngle() - myLocation.pan()) > 0.05) {
				stateOfProcess = panIsGoingToLocation;
				return 0;
			} else {
				stateOfProcess = algorithmIsPerforming;
			}
		} else if (api == API::ALGORITHMCOMM) {
			AlgorithmCommunication::LocationInformation myLocation = *(listOfLocationInformationFromAlgComm->mutable_locationinformation(locationIndex));
			panTiltRead[0] = (float)myLocation.pan(); panTiltRead[1] = (float)myLocation.tilt();
			headpt->panTiltGoPos(panTiltRead[0],panTiltRead[1]);
			if (abs(headpt->getPanAngle() - myLocation.pan()) > 0.001) {
				qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~Pan of Hawkeye and desired pan is " << headpt->getPanAngle() << myLocation.pan();
				stateOfProcess = panIsGoingToLocation;
				return 0;
			} else {
				stateOfProcess = waitForStabilView;
			}
		}
	} else if (stateOfProcess == waitForStabilView) {
		static int counter = 0;
		qDebug() << "counter is " << counter;
		if (counter < 80) {
			stateOfProcess = waitForStabilView;
			counter++;
		} else {
			stateOfProcess = algorithmIsPerforming;
			counter = 0;
		}
	}
	else if (stateOfProcess == algorithmIsPerforming) {
		AlgorithmCommunication::LocationInformation myLocation = *(listOfLocationInformationFromAlgComm->mutable_locationinformation(locationIndex));
		panTiltRead[0] = (float)myLocation.pan(); panTiltRead[1] = (float)myLocation.tilt();
		qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~Algorithm will be performed" << panTiltRead[0] << panTiltRead[1] << locationIndex << initROI;
		asel_pan_change((uchar*)buf.constData(), width, height, panTiltRead[0], panTiltRead[1], locationIndex, initROI);
		initROI = 0;
		stateOfProcess = algorithmIsCompletedMoveNextPoint;
		return 0;
	}
	else if (stateOfProcess == algorithmIsCompletedMoveNextPoint) {
		if (locationIndex == (locationSize - 1)) {
			timer.restart();
			stateOfProcess = waitForGivenInterval;
			locationIndex = 0;
			return 0;
		} else {
			++locationIndex;
			stateOfProcess = panIsGoingToLocation;
		}
	} else if (stateOfProcess == waitForGivenInterval) {
		if (!timer.isValid())
			timer.start();
		else if (timer.elapsed() >= (time * 1000)) {
			stateOfProcess = panIsGoingToLocation;
			return 0;
		}
	}
	return 0;
}

int PanChangeAlgorithmElement::release()
{
	asel_pan_change_release();
	return 0;
}

int PanChangeAlgorithmElement::setPanChangeInfo(kaapi::ListOfLocationInformation* listOfLocInfo)
{
	listOfLocationInformation->set_intervalforcirculation(listOfLocInfo->locationinformation_size());
	foreach(kaapi::LocationInformation loI , listOfLocInfo->locationinformation()) {
		kaapi::LocationInformation *loc = &loI;
		loc = listOfLocationInformation->add_locationinformation();
	}
	api = API::KARDELEN;
	return 0;
}

int PanChangeAlgorithmElement::setPanChangeInfoFrom(const AlgorithmCommunication::ListOfLocationInformation &listOfLocInfo)
{
	AlgorithmCommunication::ListOfLocationInformation loI = listOfLocInfo;
	listOfLocationInformationFromAlgComm->clear_locationinformation();
	listOfLocationInformationFromAlgComm->set_intervalforcirculation(loI.intervalforcirculation());
	for (int i = 0; i < listOfLocInfo.locationinformation().size(); ++i) {
		AlgorithmCommunication::LocationInformation *loc = listOfLocationInformationFromAlgComm->add_locationinformation();
		loc->set_pan(loI.locationinformation(i).pan());loc->set_tilt(loI.locationinformation(i).tilt());loc->set_zoom(loI.locationinformation(i).zoom());
	}
	api = API::ALGORITHMCOMM;
	return 0;
}

QString PanChangeAlgorithmElement::getTypeString()
{
	return "change detection";
}
