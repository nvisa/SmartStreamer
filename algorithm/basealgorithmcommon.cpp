#include "basealgorithmcommon.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

#include <lmm/debug.h>

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

BaseAlgorithmCommon::BaseVariables BaseAlgorithmCommon::getAlgoParameters()
{
	BaseVariables v;
	QJsonObject mainObj = readJson("algorithm_new.json");
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

int BaseAlgorithmCommon::getMotionSensitivity()
{
	QJsonObject mainObj = readJson("algorithm_new.json");
	qDebug() << mainObj;
	if (!mainObj.contains("motion_detection")) {
		qDebug() << "json file doesn't `motion_detection` object ";
		return -ENODATA;
	}
	int sens = mainObj.value("motion_detection").toObject().value("sensitivity").toInt();
	return sens;
}

bool BaseAlgorithmCommon::getMotionClassification()
{
	QJsonObject mainObj = readJson("algorithm_new.json");
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

BaseAlgorithmCommon::BaseAlgorithmCommon()
{
}

