#include "basealgorithmcommon.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

#include <lmm/debug.h>

#define FILENAME "algorithm_new.json"

static QJsonObject readJson(const QString &filename)
{
	QJsonObject obj;
	QFile f(filename);
	if (!f.open(QIODevice::ReadOnly)) {
		qDebug() << "File opening error: " << errno << filename;
		return obj;
	}
	QByteArray ba = f.readAll();
	f.close();
	QJsonDocument doc = QJsonDocument::fromJson(ba);
	obj = doc.object();
	return obj;
}

static int writeJson(const QString &filename, QJsonObject obj)
{
	QFile f(filename);
	if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug() << "File opening error: " << errno << filename;
		return errno;
	}
	f.write(QJsonDocument(obj).toJson());
	f.close();
	return 0;
}

BaseAlgorithmCommon::BaseVariables BaseAlgorithmCommon::getAlgoParameters()
{
	BaseVariables v;
	QJsonObject mainObj = readJson(FILENAME);
	qDebug() << mainObj;
	if (!mainObj.contains("parameters")) {
		qDebug() << "json file doesn't `parameters` object ";
		return v;
	}
	QJsonObject param = mainObj.value("parameters").toObject();
	v.rgb = param.value("rgb").toInt();
	v.ill = param.value("ill").toInt();
	v.debug = param.value("debug").toInt();
	v.shadow = param.value("shadow").toInt();
	v.record = param.value("record").toInt();
	v.privacy = param.value("privacy").toInt();
	v.stabilization = param.value("stabilization").toInt();
	return v;
}

int BaseAlgorithmCommon::getSensitivity(const QString &objName)
{
	QJsonObject mainObj = readJson(FILENAME);
	qDebug() << mainObj;
	if (!mainObj.contains(objName)) {
		qDebug() << "json file doesn't object " << objName;
		return -ENODATA;
	}
	int sens = mainObj.value(objName).toObject().value("sensitivity").toInt();
	return sens;
}

bool BaseAlgorithmCommon::getMotionClassification()
{
	QJsonObject mainObj = readJson(FILENAME);
	if (!mainObj.contains("motion_detection")) {
		qDebug() << "json file doesn't `motion_detection` object ";
		return -ENODATA;
	}
	int classification = mainObj.value("motion_detection").toObject().value("classification").toBool();
	return classification;
}

int BaseAlgorithmCommon::saveRoiPoints(aw::RoiQ troi)
{
	QFile f("points.txt");
	if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
		return -errno;
	QTextStream out(&f);
	out << troi.polygon().size() << "\n";
	for (int i = 0; i < troi.polygon().size(); i++) {
		aw::TPolygon myPoly = troi.polygon(i);
		out << myPoly.points_size() << "\n";
	}
	for (int i = 0; i < troi.polygon().size(); i++) {
		aw::TPolygon myPoly = troi.polygon(i);
		int state = (myPoly.is_active() == true) ? 1 : 0;
		out << state << "\n";
	}

	for (int i = 0; i < troi.polygon().size(); i++) {
		aw::TPolygon myPoly = troi.polygon(i);
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

int BaseAlgorithmCommon::setSensitivity(const QString objName, int sensitivity)
{
	QJsonObject mainObj = readJson(FILENAME);
	if (!mainObj.contains(objName)) {
		qDebug() << "json file doesn't object " << objName;
		return -ENODATA;
	}
	QJsonObject subObj = mainObj.value(objName).toObject();
	subObj.insert("sensitivity", sensitivity);
	mainObj.insert(objName, subObj);

	return writeJson(FILENAME, mainObj);
}

QJsonObject BaseAlgorithmCommon::getSubObj(const QString &objName)
{
	QJsonObject mainObj = readJson(FILENAME);
	if (!mainObj.contains(objName)) {
		qDebug() << "json file doesn't object " << objName;
		return QJsonObject();
	}
	return mainObj.value(objName).toObject();
}

int BaseAlgorithmCommon::getFaceCamID()
{
	QJsonObject subObj = getSubObj("face");
	if (!subObj.contains("cam_id"))
		return -ENODATA;
	return subObj.value("cam_id").toInt();
}

int BaseAlgorithmCommon::getFaceFrameRate()
{
	QJsonObject subObj = getSubObj("face");
	if (!subObj.contains("frame_rate"))
		return -ENODATA;
	return subObj.value("frame_rate").toInt();
}

int BaseAlgorithmCommon::setFaceCamID(int v)
{
	QJsonObject mainObj = readJson(FILENAME);
	if (!mainObj.contains("face"))
		return -ENODATA;
	QJsonObject subObj = mainObj.value("face").toObject();
	subObj.insert("cam_id", v);
	mainObj.insert("face", subObj);
	return writeJson(FILENAME, mainObj);
}

int BaseAlgorithmCommon::setFaceFrameRate(int v)
{
	QJsonObject mainObj = readJson(FILENAME);
	if (!mainObj.contains("face"))
		return -ENODATA;
	QJsonObject subObj = mainObj.value("face").toObject();
	subObj.insert("frame_rate", v);
	mainObj.insert("face", subObj);
	return writeJson(FILENAME, mainObj);
}

bool BaseAlgorithmCommon::isTrackingAuto()
{
	QJsonObject subObj = getSubObj("tracking");
	if (!subObj.contains("auto"))
		return false;
	return subObj.value("auto").toBool();
}

bool BaseAlgorithmCommon::isTrackingSemiAuto()
{
	QJsonObject subObj = getSubObj("tracking");
	if (!subObj.contains("semi_auto"))
		return false;
	return subObj.value("semi_auto").toBool();
}

bool BaseAlgorithmCommon::isTrackingManual()
{
	QJsonObject subObj = getSubObj("tracking");
	if (!subObj.contains("manual"))
		return false;
	return subObj.value("manual").toBool();
}

float BaseAlgorithmCommon::getTrackingScore()
{
	QJsonObject subObj = getSubObj("tracking");
	if (!subObj.contains("score"))
		return false;
	return subObj.value("score").toDouble();
}

int BaseAlgorithmCommon::getTrackingDuration()
{
	QJsonObject subObj = getSubObj("tracking");
	if (!subObj.contains("duration"))
		return false;
	return subObj.value("duration").toInt();
}

bool BaseAlgorithmCommon::getTrackingMultiple()
{
	QJsonObject subObj = getSubObj("tracking");
	if (!subObj.contains("multiple_track"))
		return false;
	return subObj.value("multiple_track").toBool();
}

int BaseAlgorithmCommon::setTrackingScore(float v)
{
	QJsonObject mainObj = readJson(FILENAME);
	if (!mainObj.contains("tracking"))
		return -ENODATA;
	QJsonObject subObj = mainObj.value("tracking").toObject();
	subObj.insert("score", v);
	mainObj.insert("tracking", subObj);
	return writeJson(FILENAME, mainObj);
}

int BaseAlgorithmCommon::setTrackingDuration(int v)
{
	QJsonObject mainObj = readJson(FILENAME);
	if (!mainObj.contains("tracking"))
		return -ENODATA;
	QJsonObject subObj = mainObj.value("tracking").toObject();
	subObj.insert("duration", v);
	mainObj.insert("tracking", subObj);
	return writeJson(FILENAME, mainObj);
}

int BaseAlgorithmCommon::setTrackingMultiple(bool v)
{
	QJsonObject mainObj = readJson(FILENAME);
	if (!mainObj.contains("tracking"))
		return -ENODATA;
	QJsonObject subObj = mainObj.value("tracking").toObject();
	subObj.insert("multiple_track", v);
	mainObj.insert("tracking", subObj);
	return writeJson(FILENAME, mainObj);
}

BaseAlgorithmCommon::BaseAlgorithmCommon()
{
}

