#include "motionalgorithmelement.h"

#include "lmm/debug.h"
#include "algorithmfunctions.h"
#include <QFile>
#include <QJsonObject>

MotionAlgorithmElement::MotionAlgorithmElement(QObject *parent)
	: BaseAlgorithmElement(parent)
{
	algoState = UNKNOWN;
}

int MotionAlgorithmElement::init()
{
	v.debug = 0;
	v.ill = 1;
	v.privacy = 0;
	v.record = 0;
	v.rgb = 1;
	v.shadow = 0;
	v.stabilization = 0;
	control.initialize = 1;

	return BaseAlgorithmElement::init();
}

int MotionAlgorithmElement::processAlgo(const RawBuffer &buf)
{
	mInfo("Processing Algorithm %d", buf.constPars()->videoHeight);
	QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);

	int width = buf.constPars()->videoWidth;
	int height = buf.constPars()->videoHeight;

#if HAVE_VIA_MOTION
#if HAVE_TK1
	asel_via_base((uchar*)buf.constData(), width * height * 3 / 2, width, height,
				  v.rgb, v.record, v.shadow, v.ill, v.debug, v.stabilization, v.privacy,
				  control.meta, control.panTiltZoomRead, control.alarmFlag,
				  control.initialize, control.sensitivity);
#elif HAVE_TX1
	asel_via_base((uchar*)buf.constData(), width * height, width, height,
				  v.rgb, v.shadow, v.ill, v.debug, v.stabilization, v.privacy,
				  control.meta, control.panTiltZoomRead, control.alarmFlag,
				  control.initialize, control.sensitivity,control.classification, false);
#endif // HAVE_TK1
#endif // HAVE_VIA_MOTION

	if (control.initialize)
		control.initialize = 0;
	QByteArray ba = QByteArray((char *)control.meta, 4096);
	hash.insert("motion_results", ba);
	((RawBuffer *)&buf)->pars()->metaData = RawBuffer::serializeMetadata(hash);

	return 0;
}

int MotionAlgorithmElement::release()
{
#if HAVE_VIA_MOTION
	asel_via_base_release();
#endif
	return 0;
}

int MotionAlgorithmElement::getSensitivity()
{
	return control.sensitivity;
}

bool MotionAlgorithmElement::getClassification()
{
	return control.classification;
}

void MotionAlgorithmElement::setSensitivity(int value)
{
	control.sensitivity = value;
}

void MotionAlgorithmElement::setClassification(bool value)
{
	control.classification = value;
}

QString MotionAlgorithmElement::getTypeString()
{
	return "motion";
}

int MotionAlgorithmElement::reloadJson(const QJsonObject &node)
{
	control.sensitivity = node["sensitivity"].toInt();
	control.classification = node["classification"].toBool();
	return 0;
}

QJsonObject MotionAlgorithmElement::resaveJson(const QJsonObject &node)
{
	QJsonObject tr = node;
	tr["sensitivity"] = control.sensitivity;
	tr["classification"] = control.classification;
	return tr;
}

int MotionAlgorithmElement::saveROI(AlgorithmCommunication::TRoi troi)
{
	QFile f("points.txt");
	if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
		return -errno;
	QTextStream out(&f);
	out << troi.polygon().size() << "\n";
	for (int i = 0; i < troi.polygon().size(); i++) {
		AlgorithmCommunication::TPolygon myPoly = troi.polygon(i);
		out << myPoly.points_size() << "\n";
	}
	for (int i = 0; i < troi.polygon().size(); i++) {
		AlgorithmCommunication::TPolygon myPoly = troi.polygon(i);
		int state = (myPoly.is_active() == true) ? 1 : 0;
		out << state << "\n";
	}
	for (int i = 0; i < troi.polygon().size(); i++) {
		AlgorithmCommunication::TPolygon myPoly = troi.polygon(i);
		for (int k = 0; k < myPoly.points_size(); k++) {
			out << myPoly.points(k).x() << "\n";
			out << myPoly.points(k).y() << "\n";
		}
	}
	// rect1
	out << troi.rect1().upperleft().x() << "\n";
	out << troi.rect1().upperleft().y() << "\n";
	out << troi.rect1().bottomright().x() << "\n";
	out << troi.rect1().bottomright().y() << "\n";

	// rect2
	out << troi.rect2().upperleft().x() << "\n";
	out << troi.rect2().upperleft().y() << "\n";
	out << troi.rect2().bottomright().x() << "\n";
	out << troi.rect2().bottomright().y() << "\n";
	f.close();
	return 0;
}

#if HAVE_TK1
int MotionAlgorithmElement::saveROI(const OrionCommunication::TRoi *troi)
{
	QFile f("points.txt");
	if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
		return -errno;
	QTextStream out(&f);
	out << troi->polygon().size() << "\n";
	for (int i = 0; i < troi->polygon().size(); i++) {
		OrionCommunication::TPolygon myPoly = troi->polygon(i);
		out << myPoly.points_size() << "\n";
	}
	for (int i = 0; i < troi->polygon().size(); i++) {
		OrionCommunication::TPolygon myPoly = troi->polygon(i);
		int state = (myPoly.is_active() == true) ? 1 : 0;
		out << state << "\n";
	}
	for (int i = 0; i < troi->polygon().size(); i++) {
		OrionCommunication::TPolygon myPoly = troi->polygon(i);
		for (int k = 0; k < myPoly.points_size(); k++) {
			out << myPoly.points(k).x() << "\n";
			out << myPoly.points(k).y() << "\n";
		}
	}
	// rect1
	out << troi->rect1().upperleft().x() << "\n";
	out << troi->rect1().upperleft().y() << "\n";
	out << troi->rect1().bottomright().x() << "\n";
	out << troi->rect1().bottomright().y() << "\n";

	// rect2
	out << troi->rect2().upperleft().x() << "\n";
	out << troi->rect2().upperleft().y() << "\n";
	out << troi->rect2().bottomright().x() << "\n";
	out << troi->rect2().bottomright().y() << "\n";
	f.close();
}
#endif

AlgorithmCommunication::MotionParameters* MotionAlgorithmElement::loadROI()
{
	AlgorithmCommunication::MotionParameters *motionParam = new AlgorithmCommunication::MotionParameters();
	QFile f("points.txt");
	if (!f.open(QIODevice::ReadOnly))
		return motionParam;
	QTextStream in(&f);
	QStringList lines;
	while (!in.atEnd())
		lines << in.readLine();
	int numOfPolySize = lines.at(0).toInt();
	QList<int> pointSize;
	for (int i = 0; i < numOfPolySize; i++)
		pointSize << lines.at(i + 1).toInt();
	QList<int> polyState;
	for (int i = 0; i < numOfPolySize; i++)
		polyState << lines.at(i + 1 + numOfPolySize).toInt();
	int polyCounter = 0;
	AlgorithmCommunication::TRoi *myRoi = new AlgorithmCommunication::TRoi();
	for (int j = 0; j < numOfPolySize; j++) {
		AlgorithmCommunication::TPolygon *myPoly1 = myRoi->add_polygon();
		bool state = (polyState[j] == 1) ? true : false;
		myPoly1->set_is_active(state);
		for (int i = 0; i < pointSize[j] * 2; i = i + 2) {
			AlgorithmCommunication::TPoint *myPoints = myPoly1->add_points();
			myPoints->set_x(lines.at(polyCounter + i + 1  + 2 * numOfPolySize).toFloat());
			myPoints->set_y(lines.at(polyCounter + i + 2  + 2 * numOfPolySize).toFloat());
		}
		polyCounter = polyCounter + 2 * pointSize[j];
	}
	AlgorithmCommunication::TRectangle *myRect1 =  myRoi->mutable_rect1();
	AlgorithmCommunication::TRectangle *myRect2 =  myRoi->mutable_rect2();
	myRect1->mutable_upperleft()->set_x(lines.at(lines.size() - 8).toFloat());
	myRect1->mutable_upperleft()->set_y(lines.at(lines.size() - 7).toFloat());
	myRect1->mutable_bottomright()->set_x(lines.at(lines.size() - 6).toFloat());
	myRect1->mutable_bottomright()->set_y(lines.at(lines.size() - 5).toFloat());
	myRect2->mutable_upperleft()->set_x(lines.at(lines.size() - 4).toFloat());
	myRect2->mutable_upperleft()->set_y(lines.at(lines.size() - 3).toFloat());
	myRect2->mutable_bottomright()->set_x(lines.at(lines.size() - 2).toFloat());
	myRect2->mutable_bottomright()->set_y(lines.at(lines.size() - 1).toFloat());
	motionParam->set_allocated_roilist(myRoi);
	return motionParam;
}

#if HAVE_TK1
OrionCommunication::TRoi *MotionAlgorithmElement::loadROIOrion()
{
	OrionCommunication::TRoi *myRoi = new OrionCommunication::TRoi();
	QFile f("points.txt");
	if (!f.open(QIODevice::ReadOnly))
		return myRoi;
	QTextStream in(&f);
	QStringList lines;
	while (!in.atEnd())
		lines << in.readLine();
	int numOfPolySize = lines.at(0).toInt();
	QList<int> pointSize;
	for (int i = 0; i < numOfPolySize; i++)
		pointSize << lines.at(i + 1).toInt();
	QList<int> polyState;
	for (int i = 0; i < numOfPolySize; i++)
		polyState << lines.at(i + 1 + numOfPolySize).toInt();
	int polyCounter = 0;
	for (int j = 0; j < numOfPolySize; j++) {
		OrionCommunication::TPolygon *myPoly1 = myRoi->add_polygon();
		bool state = (polyState[j] == 1) ? true : false;
		myPoly1->set_is_active(state);
		for (int i = 0; i < pointSize[j] * 2; i = i + 2) {
			OrionCommunication::TPoint *myPoints = myPoly1->add_points();
			myPoints->set_x(lines.at(polyCounter + i + 1  + 2 * numOfPolySize).toFloat());
			myPoints->set_y(lines.at(polyCounter + i + 2  + 2 * numOfPolySize).toFloat());
		}
		polyCounter = polyCounter + 2 * pointSize[j];
	}
	OrionCommunication::TRectangle *myRect1 =  myRoi->mutable_rect1();
	OrionCommunication::TRectangle *myRect2 =  myRoi->mutable_rect2();
	myRect1->mutable_upperleft()->set_x(lines.at(lines.size() - 8).toFloat());
	myRect1->mutable_upperleft()->set_y(lines.at(lines.size() - 7).toFloat());
	myRect1->mutable_bottomright()->set_x(lines.at(lines.size() - 6).toFloat());
	myRect1->mutable_bottomright()->set_y(lines.at(lines.size() - 5).toFloat());
	myRect2->mutable_upperleft()->set_x(lines.at(lines.size() - 4).toFloat());
	myRect2->mutable_upperleft()->set_y(lines.at(lines.size() - 3).toFloat());
	myRect2->mutable_bottomright()->set_x(lines.at(lines.size() - 2).toFloat());
	myRect2->mutable_bottomright()->set_y(lines.at(lines.size() - 1).toFloat());
	return myRoi;
}
#endif
