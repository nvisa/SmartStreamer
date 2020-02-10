#include "panchangealgorithmelement.h"
#include "algorithmfunctions.h"
#include "applicationinfo.h"
#include <ecl/ptzp/ptzphead.h>
#include <ecl/ptzp/ptzpdriver.h>

#include "lmm/debug.h"
#include "kardelenapi.h"

#include <QFile>
#include <QFileInfo>
#include <string>

PanChangeAlgorithmElement::PanChangeAlgorithmElement(QObject *parent)
	: BaseAlgorithmElement(parent)
{
	algoState = UNKNOWN;
	listOfLocationInformationFromAlgComm = new AlgorithmCommunication::ListOfLocationInformation();
	loadLocations();
}

int PanChangeAlgorithmElement:: init()
{
	mInfo("Initializing PanChange Algorithm");
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
	if (locationSize == 0)
		return 0;
	time = listOfLocationInformationFromAlgComm->intervalforcirculation();

	float panTiltRead[2];
	if (stateOfProcess == panIsGoingToLocation) {
		mInfo("Currently going to pan location");
		AlgorithmCommunication::LocationInformation myLocation = *(listOfLocationInformationFromAlgComm->mutable_locationinformation(locationIndex));
		panTiltRead[0] = (float)myLocation.pan(); panTiltRead[1] = (float)myLocation.tilt();
		headz->setProperty(4,myLocation.zoom());
		headpt->panTiltGoPos(panTiltRead[0],panTiltRead[1]);
		if (abs(headpt->getPanAngle() - myLocation.pan()) > 0.001) {
			mInfo("Not arrived the pan location");
			stateOfProcess = panIsGoingToLocation;
			return 0;
		} else {
			mInfo("State is changed to waitForStabilView");
			stateOfProcess = waitForStabilView;
		}
	} else if (stateOfProcess == waitForStabilView) {
		static int counter = 0;
		if (counter < 200) {
			mInfo("Waiting for stabil view");
			stateOfProcess = waitForStabilView;
			counter++;
		} else {
			mInfo("Going into algorithm performing state");
			stateOfProcess = algorithmIsPerforming;
			counter = 0;
		}
	} else if (stateOfProcess == algorithmIsPerforming) {
		AlgorithmCommunication::LocationInformation myLocation = *(listOfLocationInformationFromAlgComm->mutable_locationinformation(locationIndex));
		panTiltRead[0] = (float)myLocation.pan(); panTiltRead[1] = (float)myLocation.tilt();
#if HAVE_TX1
		mInfo("Before asel_pan_change");
		asel_pan_change((uchar*)buf.constData(), width, height, panTiltRead[0], panTiltRead[1], locationIndex, initROI);
		mInfo("After asel_pan_change");
		int counter = numberOfTurnAtGivenIndex.at(locationIndex);
		QString fileInitial("/home/nvidia/Pictures/ChangeLogs/res");
		QFile f(QString("%1_%2_%3_diff.png").arg(fileInitial).arg(locationIndex).arg(counter));
		mInfo("Looking for file");
		if(f.open(QIODevice::ReadOnly)) {
			const QByteArray &imdata = f.readAll();
			f.close();
			if(KardelenAPIServer::instance())
				KardelenAPIServer::instance()->setPanChangeFrame(QString("index%1").arg(locationIndex).toStdString(), imdata);
			//QProcess::startDetached(QString("rm %1").arg(QString("%1_%2_%3_diff.png").arg(fileInitial).arg(locationIndex).arg(counter)));
			counter++;
			if (counter == 10)
				counter = 0;
			numberOfTurnAtGivenIndex[locationIndex] = counter;
		}
#endif
	initROI = 0;
	stateOfProcess = algorithmIsCompletedMoveNextPoint;
	mInfo("Moving to algorithmIsCompletedMoveNextPoint");
	return 0;
} else if (stateOfProcess == algorithmIsCompletedMoveNextPoint) {
		mInfo("stateOfProcess of algorithmIsCompletedMoveNextPoint");
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
	numberOfTurnAtGivenIndex.clear();
	if (listOfLocInfo.locationinformation().size() == 0) {
		return -1;
	}
	listOfLocationInformationFromAlgComm->clear_locationinformation();
	listOfLocationInformationFromAlgComm->set_intervalforcirculation(loI.intervalforcirculation());
	for (int i = 0; i < listOfLocInfo.locationinformation().size(); ++i) {
		qDebug() << " location information of " << i << " arrived";
		AlgorithmCommunication::LocationInformation *loc = listOfLocationInformationFromAlgComm->add_locationinformation();
		loc->set_pan(loI.locationinformation(i).pan());loc->set_tilt(loI.locationinformation(i).tilt());loc->set_zoom(loI.locationinformation(i).zoom());
		numberOfTurnAtGivenIndex.push_back(1);
		qDebug() << " location information of " << i << " set";
	}
	saveLocations();
	return 0;
}

QString PanChangeAlgorithmElement::getTypeString()
{
	return "change detection";
}

int PanChangeAlgorithmElement::saveLocations()
{
	std::string str;
	listOfLocationInformationFromAlgComm->SerializeToString(&str);
	QFile file("locations.bin");
	if (!file.open(QIODevice::WriteOnly)) {
		return -EPERM;
	}
	file.write(str.data(), str.size());
	file.close();
	qDebug() << "saving is completed";
	return 0;
}

int PanChangeAlgorithmElement::loadLocations()
{
	locationIndex = 0;
	if (!QFileInfo("locations.bin").exists()) {
		mInfo("The file doesn't existed");
		return -1;
	}
	QFile f("locations.bin");
	if (!f.open(QIODevice::ReadOnly))
		return -EPERM;
	QByteArray ba = f.readAll();
	listOfLocationInformationFromAlgComm->ParseFromString(std::string(ba.data(), ba.size()));
	for (int i = 0; i < listOfLocationInformationFromAlgComm->locationinformation_size(); i++)
		numberOfTurnAtGivenIndex.push_back(1);
	QProcess::execute("bash -c \"rm -f /home/nvidia/Pictures/ChangeLogs/*\"");
	f.close();
	return 0;
}
