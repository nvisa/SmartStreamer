#include "applicationinfo.h"

#define FILENAME "application_info.json"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QFile>


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

bool ApplicationInfo::isBotasFixEnabled()
{
	QJsonObject obj = getSubObj("botas_fix");
	if (obj.isEmpty())
		return false;
	return obj.value("enable").toBool();
}

bool ApplicationInfo::isBotasDomeEnabled()
{
	QJsonObject obj = getSubObj("botas_dome");
	if (obj.isEmpty())
		return false;
	return obj.value("enable").toBool();
}

QString ApplicationInfo::getBotasFixPtAddress()
{
	QJsonObject obj = getSubObj("botas_fix");
	if (obj.isEmpty())
		return "";
	return obj.value("pt_address").toString();
}

QString ApplicationInfo::getBotasDomePtAddress()
{
	QJsonObject obj = getSubObj("botas_dome");
	if (obj.isEmpty())
		return "";
	return obj.value("pt_address").toString();
}

int ApplicationInfo::getBotasFixPtGrpcPort()
{
	QJsonObject obj = getSubObj("botas_fix");
	if (obj.isEmpty())
		return -ENODATA;
	return obj.value("pt_grpc_port").toInt();
}

int ApplicationInfo::getBotasDomePtGrpcPort()
{
	QJsonObject obj = getSubObj("botas_dome");
	if (obj.isEmpty())
		return -ENODATA;
	return obj.value("pt_grpc_port").toInt();
}

QString ApplicationInfo::getBotasFixStreamAddress()
{
	QJsonObject obj = getSubObj("botas_fix");
	if (obj.isEmpty())
		return "";
	return obj.value("stream_address").toString();
}

QString ApplicationInfo::getBotasDomeStreamAddress()
{
	QJsonObject obj = getSubObj("botas_dome");
	if (obj.isEmpty())
		return "";
	return obj.value("stream_address").toString();
}

QString ApplicationInfo::getBotasFixAlgorithms()
{
	QJsonObject obj = getSubObj("botas_fix");
	if (obj.isEmpty())
		return "";
	return obj.value("algorithms").toString();
}

QJsonObject ApplicationInfo::getSubObj(const QString &objName)
{
	QJsonObject mainObj = readJson(FILENAME);
	if (!mainObj.contains(objName)) {
		qDebug() << "json file doesn't object " << objName;
		return QJsonObject();
	}
	return mainObj.value(objName).toObject();
}

ApplicationInfo::ApplicationInfo()
{

}

