#include "panchangealgorithmelement.h"
#include "algorithmfunctions.h"
#include "applicationinfo.h"
#include <ecl/ptzp/ptzphead.h>
#include <ecl/ptzp/ptzpdriver.h>

#include "lmm/debug.h"
#include "kardelenapi.h"

#include <QFile>

PanChangeAlgorithmElement::PanChangeAlgorithmElement(QObject *parent)
	: BaseAlgorithmElement(parent)
{
	algoState = UNKNOWN;
	listOfLocationInformationFromAlgComm = new AlgorithmCommunication::ListOfLocationInformation();
}

int PanChangeAlgorithmElement:: init()
{
	initROI = 1;
	locationIndex = 0;
	stateOfProcess = panIsGoingToLocation;
	return BaseAlgorithmElement::init();
}

int PanChangeAlgorithmElement::processAlgo(const RawBuffer &buf)
{
	mInfo("Processing Algorithm %d", buf.constPars()->videoHeight);
	PtzpHead *headpt = ApplicationInfo::instance()->getPtzpDriver(0)->getHead(1);
	PtzpHead *headz = ApplicationInfo::instance()->getPtzpDriver(0)->getHead(0);
	int width = buf.constPars()->videoWidth;
	int height = buf.constPars()->videoHeight;
	static int locationSize;

	static long time;
	locationSize = listOfLocationInformationFromAlgComm->locationinformation_size();
	time = listOfLocationInformationFromAlgComm->intervalforcirculation();

	float panTiltRead[2];
	if (stateOfProcess == panIsGoingToLocation) {
		AlgorithmCommunication::LocationInformation myLocation = *(listOfLocationInformationFromAlgComm->mutable_locationinformation(locationIndex));
		panTiltRead[0] = (float)myLocation.pan(); panTiltRead[1] = (float)myLocation.tilt();
		headz->setProperty(4,myLocation.zoom());
		headpt->panTiltGoPos(panTiltRead[0],panTiltRead[1]);
		if (abs(headpt->getPanAngle() - myLocation.pan()) > 0.001) {
			stateOfProcess = panIsGoingToLocation;
			return 0;
		} else {
			stateOfProcess = waitForStabilView;
		}
	} else if (stateOfProcess == waitForStabilView) {
		static int counter = 0;
		if (counter < 200) {
			stateOfProcess = waitForStabilView;
			counter++;
		} else {
			stateOfProcess = algorithmIsPerforming;
			counter = 0;
		}
	} else if (stateOfProcess == algorithmIsPerforming) {
		AlgorithmCommunication::LocationInformation myLocation = *(listOfLocationInformationFromAlgComm->mutable_locationinformation(locationIndex));
		panTiltRead[0] = (float)myLocation.pan(); panTiltRead[1] = (float)myLocation.tilt();
#if HAVE_TX1
		asel_pan_change((uchar*)buf.constData(), width, height, panTiltRead[0], panTiltRead[1], locationIndex, initROI);
		int counter = numberOfTurnAtGivenIndex.at(locationIndex);
		QString fileInitial("/home/nvidia/Pictures/ChangeLogs/res");
		QFile f(QString("%1_%2_%3_diff.png").arg(fileInitial).arg(locationIndex).arg(counter));
		qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << QString("%1_%2_%3_diff.png").arg(fileInitial).arg(locationIndex).arg(counter);
		if(f.open(QIODevice::ReadOnly)) {
			const QByteArray &imdata = f.readAll();
			f.close();
			KardelenAPIServer::instance()->setPanChangeFrame("", imdata);
			//QProcess::startDetached(QString("rm %1").arg(QString("%1_%2_%3_diff.png").arg(fileInitial).arg(locationIndex).arg(counter)));
			counter++;
			if (counter == 10)
				counter = 0;
			numberOfTurnAtGivenIndex[locationIndex] = counter;
		}
#endif
	initROI = 0;
	stateOfProcess = algorithmIsCompletedMoveNextPoint;
	return 0;
} else if (stateOfProcess == algorithmIsCompletedMoveNextPoint) {
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
	else if (timer.elapsed() >= (time)) {
		stateOfProcess = panIsGoingToLocation;
		return 0;
	}
}
#if 0
if (KardelenAPIServer::instance()) {
	QFile f("/home/nvidia/Pictures/ChangeLogs/res_%1_%2_%3_diff.png");
	f.open(QIODevice::ReadOnly);
	const QByteArray &imdata = f.readAll();
	f.close();
	KardelenAPIServer::instance()->setPanChangeFrame("", imdata);
}
#endif
return 0;
}

int PanChangeAlgorithmElement::release()
{
#if HAVE_TX1
	asel_pan_change_release();
	for (int i = 0; i < numberOfTurnAtGivenIndex.size(); ++i)
		numberOfTurnAtGivenIndex[i] = 1;
	QProcess::execute("bash -c \"rm -f /home/nvidia/Pictures/ChangeLogs/*\"");
#endif
	return 0;
}

int PanChangeAlgorithmElement::setPanChangeInfoFrom(const AlgorithmCommunication::ListOfLocationInformation &listOfLocInfo)
{
	AlgorithmCommunication::ListOfLocationInformation loI = listOfLocInfo;
	listOfLocationInformationFromAlgComm->clear_locationinformation();
	listOfLocationInformationFromAlgComm->set_intervalforcirculation(loI.intervalforcirculation());
	numberOfTurnAtGivenIndex.clear();
	for (int i = 0; i < listOfLocInfo.locationinformation().size(); ++i) {
		qDebug() << " location information of " << i << " arrived";
		AlgorithmCommunication::LocationInformation *loc = listOfLocationInformationFromAlgComm->add_locationinformation();
		loc->set_pan(loI.locationinformation(i).pan());loc->set_tilt(loI.locationinformation(i).tilt());loc->set_zoom(loI.locationinformation(i).zoom());
		numberOfTurnAtGivenIndex.push_back(1);
		qDebug() << " location information of " << i << " set";
	}
	return 0;
}

QString PanChangeAlgorithmElement::getTypeString()
{
	return "change detection";
}
