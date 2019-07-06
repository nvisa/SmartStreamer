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
	static int locationIndex = 0;
	static long time;
	locationSize = listOfLocationInformationFromAlgComm->locationinformation_size();
	time = listOfLocationInformationFromAlgComm->intervalforcirculation();

	float panTiltRead[2];
	qDebug() << " Inside the process Algo";
	if (stateOfProcess == panIsGoingToLocation) {
			AlgorithmCommunication::LocationInformation myLocation = *(listOfLocationInformationFromAlgComm->mutable_locationinformation(locationIndex));
			panTiltRead[0] = (float)myLocation.pan(); panTiltRead[1] = (float)myLocation.tilt();
			headpt->panTiltGoPos(panTiltRead[0],panTiltRead[1]);
			headz->setProperty(4,myLocation.zoom());
			if (abs(headpt->getPanAngle() - myLocation.pan()) > 0.001) {
				qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~Pan of Hawkeye and desired pan is " << headpt->getPanAngle() << myLocation.pan();
				stateOfProcess = panIsGoingToLocation;
				return 0;
			} else {
				stateOfProcess = waitForStabilView;
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
			qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~Algorithm will be performed" << QString::number(panTiltRead[0],'g',5) << QString::number(panTiltRead[1],'g',5) << locationIndex << initROI;
#if HAVE_TX1
			asel_pan_change((uchar*)buf.constData(), width, height, panTiltRead[0], panTiltRead[1], locationIndex, initROI);
			qDebug() << "~~~~~~~~~~~~~~~~~~~after asel_pan_change";
			if(turnIndexInfo.keys().contains(QPair<float,float>(panTiltRead[0], panTiltRead[1]))) {
				qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~" << QString("%1").arg(panTiltRead[0]) << QString::number(panTiltRead[0],'g',5);
				int counter = numberOfTurnAtGivenIndex.at(turnIndexInfo.value(QPair<float,float>(panTiltRead[0], panTiltRead[1])));
				QString panS = QString::number(panTiltRead[0],'g',5);
				QString tiltS = QString::number(panTiltRead[1],'g',5);
				QString fileInitial("/home/nvidia/Pictures/ChangeLogs/res");
				qDebug() << "~~~~~~~~~~~~~~~~~~~assigning the basic strings";
				QFile f(QString("%1_%2_%3_%4_diff.png").arg(fileInitial).arg(panS).arg(tiltS).arg(counter));
				qDebug() << " file name is " << QString("%1_%2_%3_%4_diff.png").arg(fileInitial).arg(panS).arg(tiltS).arg(counter);
				f.open(QIODevice::ReadOnly);
				const QByteArray &imdata = f.readAll();
				f.close();
				KardelenAPIServer::instance()->setPanChangeFrame("", imdata);
				counter++;
				numberOfTurnAtGivenIndex[turnIndexInfo.value(QPair<float,float>(panTiltRead[0], panTiltRead[1]))] = counter;
			}
#endif
		initROI = 0;
		stateOfProcess = algorithmIsCompletedMoveNextPoint;
		return 0;
	}
	else if (stateOfProcess == algorithmIsCompletedMoveNextPoint) {
		qDebug() << "algorithmIsCompletedMoveNextPoint";
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
		qDebug() << "waitForGivenInterval " << time;
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
#endif
	return 0;
}

int PanChangeAlgorithmElement::setPanChangeInfoFrom(const AlgorithmCommunication::ListOfLocationInformation &listOfLocInfo)
{
	AlgorithmCommunication::ListOfLocationInformation loI = listOfLocInfo;
	listOfLocationInformationFromAlgComm->clear_locationinformation();
	turnIndexInfo.clear();
	listOfLocationInformationFromAlgComm->set_intervalforcirculation(loI.intervalforcirculation());
	for (int i = 0; i < listOfLocInfo.locationinformation().size(); ++i) {
		qDebug() << " location information of " << i << " arrived";
		AlgorithmCommunication::LocationInformation *loc = listOfLocationInformationFromAlgComm->add_locationinformation();
		loc->set_pan(loI.locationinformation(i).pan());loc->set_tilt(loI.locationinformation(i).tilt());loc->set_zoom(loI.locationinformation(i).zoom());
		turnIndexInfo.insert(QPair<float,float>(loI.locationinformation(i).pan(),loI.locationinformation(i).tilt()),i);
		numberOfTurnAtGivenIndex.push_back(0);
		qDebug() << " location information of " << i << " set";
	}
	return 0;
}

QString PanChangeAlgorithmElement::getTypeString()
{
	return "change detection";
}
