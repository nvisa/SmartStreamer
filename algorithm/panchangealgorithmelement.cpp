
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

	static int locationSize = listOfLocationInformation->locationinformation_size();
	static int locationIndex = 0;
	static long time = listOfLocationInformation->intervalforcirculation();
	float panTiltRead[2];

	if (stateOfProcess == panIsGoingToLocation) {
		kaapi::LocationInformation myLocation = *(listOfLocationInformation->mutable_locationinformation(locationIndex));
		panTiltRead[0] = (float)myLocation.pan(); panTiltRead[1] = (float)myLocation.tilt();
		headpt->panTiltGoPos(panTiltRead[0],panTiltRead[1]);
		if (abs(headpt->getPanAngle() - myLocation.pan()) > 0.05) {
			stateOfProcess = panIsGoingToLocation;
			return 0;
		} else {
			stateOfProcess = algorithmIsPerforming;
		}
	}
	else if (stateOfProcess == algorithmIsPerforming) {
		asel_pan_change((uchar*)buf.constData(), width, height, panTiltRead[0], panTiltRead[1], locationIndex, initROI);
		initROI = 0;
		stateOfProcess = algorithmIsCompletedMoveNextPoint;
		return 0;
	}
	else if (stateOfProcess == algorithmIsCompletedMoveNextPoint) {
		initROI = 1;
		if (locationIndex == (locationSize - 1)) {
			stateOfProcess = waitForGivenInterval;
			locationIndex = 0;
			return 0;
		}
		++locationIndex;
	} else if (stateOfProcess == waitForGivenInterval) {
		if (!timer.isValid())
			timer.start();
		if (timer.elapsed() < time) {
			return 0;
		} else if (timer.elapsed() >= time) {
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
}

QString PanChangeAlgorithmElement::getTypeString()
{
	return "change detection";
}
