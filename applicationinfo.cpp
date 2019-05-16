#include "applicationinfo.h"
#include "yamgozstreamer.h"
#include "analogstreamer.h"
#include "ipstreamer.h"
#include "usbstreamer.h"
#include "algorithm/motionalgorithmelement.h"

#define FILENAME "application_info.json"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QFile>

#include <ecl/ptzp/kayidriver.h>
#include <ecl/ptzp/tbgthdriver.h>
#include <ecl/ptzp/aryadriver.h>
#include <ecl/ptzp/irdomedriver.h>

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

static QJsonObject getSubObj(const QString &objName)
{
	QJsonObject mainObj = readJson(FILENAME);
	if (!mainObj.contains(objName)) {
		qDebug() << "json file doesn't object " << objName;
		return QJsonObject();
	}
	return mainObj.value(objName).toObject();
}

bool ApplicationInfo::isGuiApplication()
{
	return true;
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

PtzpDriver *ApplicationInfo::getPtzpDriver(int index)
{
	if (index < drivers.size())
		return drivers[index];
	return nullptr;
}

BaseStreamer *ApplicationInfo::createAppStreamer()
{
	BaseStreamer *streamer = nullptr;
	QJsonObject obj = readJson("/etc/smartstreamer/smartconfig.json");
	if (obj.value("ipstreamer").toBool()) {
		qDebug() << "starting ip stramer";
		IpStreamer *ipStr = new IpStreamer;
		ipStr->generatePipelineForOneSource("");
		streamer = ipStr;
	} else if (obj.value("analogstreamer").toBool()) {
		qDebug() << "starting analog streamer";
		streamer = new AnalogStreamer(obj["analog_config"].toObject());
	} else if (obj.value("yamgozstreamer").toBool()) {
		streamer = new YamgozStreamer(obj["yamgoz_config"].toObject());
	} else {
		qDebug() << "starting usb streamer";
		UsbStreamer *usbStr = new UsbStreamer;
		usbStr->generatePipelineForOneSource();
		streamer = usbStr;
	}
	return streamer;
}

BaseAlgorithmElement *ApplicationInfo::createAlgorithm(int index)
{
	QJsonObject obj = readJson("/etc/smartstreamer/algodesc.json");
	QJsonArray arr = obj["algorithms"].toArray();
	if (index >= arr.size())
		return nullptr;
	QJsonObject algo = arr[index].toObject();
	if (algo["type"] == "motion") {
		MotionAlgorithmElement *motion = new MotionAlgorithmElement;
		motion->setSensitivity(algo["sensitivity"].toInt());
		motion->setClassification(algo["classification"].toBool());
		return motion;
	}

	return nullptr;
}

ApplicationInfo::ApplicationInfo()
{
	/* PTZP driver management */
	QJsonObject obj = readJson("/etc/smartstreamer/smartconfig.json");
	QJsonArray arr = obj["ptzp"].toArray();
	foreach (QJsonValue val, arr) {
		QJsonObject obj = val.toObject();
		PtzpDriver *driver = NULL;
		if (obj["type"] == "kayi") {
			driver = new KayiDriver;
		} else if (obj["type"] == "tbgth") {
			driver = new TbgthDriver(true);
		} else if (obj["type"] == "arya") {
			driver = new AryaDriver;
		} else if (obj["type"] == "irdome") {
			driver = new IRDomeDriver;
		}
		driver->setTarget(obj["target"].toString());
		if (obj.contains("grpc_port"))
			driver->startGrpcApi(obj["grpc_port"].toInt());
		drivers << driver;
	}
}

