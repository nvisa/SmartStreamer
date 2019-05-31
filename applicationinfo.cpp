#include "applicationinfo.h"
#include "yamgozstreamer.h"
#include "analogstreamer.h"
#include "ipstreamer.h"
#include "usbstreamer.h"
#include "algorithm/motionalgorithmelement.h"
#include "algorithm/stabilizationalgorithmelement.h"
#include "algorithm/trackalgorithmelement.h"

#define FILENAME "application_info.json"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QFile>
#include <QDir>

#include <ecl/ptzp/kayidriver.h>
#include <ecl/ptzp/tbgthdriver.h>
#include <ecl/ptzp/aryadriver.h>
#include <ecl/ptzp/irdomedriver.h>
#include <ecl/ptzp/yamgozdriver.h>
#include <ecl/drivers/systeminfo.h>

#include <unistd.h>

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
	return false;
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

int ApplicationInfo::startPtzpDriver()
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
		} else if (obj["type"] == "yamgoz") {
			driver = new YamGozDriver;
		}
		if (driver) {
			fDebug("Starting PTZP driver for %s", qPrintable(obj["type"].toString()));
			driver->setTarget(obj["target"].toString());
			if (obj.contains("grpc_port"))
				driver->startGrpcApi(obj["grpc_port"].toInt());
			drivers << driver;
		}
	}
	return 0;
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
		streamer = usbStr;
	}
	ffDebug() << "Starting application under path" << QDir::currentPath();
	return streamer;
}

BaseAlgorithmElement *ApplicationInfo::createAlgorithm(int index)
{
	QJsonObject obj = readJson("/etc/smartstreamer/algodesc.json");
	QJsonArray arr = obj["algorithms"].toArray();
	if (index >= arr.size())
		return new BaseAlgorithmElement;
	QJsonObject algo = arr[index].toObject();
	BaseAlgorithmElement * el = createAlgorithmFromJson(algo);
	el->setJsonAlgorithmIndex(index);
	el->reloadJson();
	return el;
}

BaseAlgorithmElement *ApplicationInfo::createAlgorithmFromJson(const QJsonObject &algo)
{
	if (algo["type"] == "motion") {
		return new MotionAlgorithmElement;
	} else if (algo["type"] == "bypass") {
		return new StabilizationAlgorithmElement;
	} else if (algo["type"] == "privacy") {
		return new StabilizationAlgorithmElement;
	} else if (algo["type"] == "track") {
		return new TrackAlgorithmElement;
	}

	return new BaseAlgorithmElement;
}

BaseAlgorithmElement *ApplicationInfo::createAlgorithm(const QString &type, int index)
{
	QJsonObject obj = readJson("/etc/smartstreamer/algodesc.json");
	QJsonArray arr = obj["algorithms"].toArray();
	QJsonArray filtered;
	QHash<int, int> imap;
	int i = 0;
	foreach (QJsonValue value, arr) {
		i++;
		QJsonObject alg = value.toObject();
		if (alg["type"] != type)
			continue;
		filtered << alg;
		imap.insert(filtered.size() - 1, i - 1);
	}

	if (filtered.size() <= index)
		return new BaseAlgorithmElement;

	QJsonObject algo = filtered[index].toObject();
	BaseAlgorithmElement *el = createAlgorithmFromJson(algo);
	el->setJsonAlgorithmIndex(imap[index]);
	el->reloadJson();
	return el;
}

void ApplicationInfo::checkStartupDelay()
{
	QJsonObject obj = readJson("/etc/smartstreamer/smartconfig.json");
	if (!obj.contains("startup_delay"))
		return;
	int sec = obj["startup_delay"].toInt() - SystemInfo::getUptime();
	if (sec > 0) {
		qDebug("waiting application start-up delay, sleeping %d msecs", sec);
		sleep(sec);
	}
}

QString ApplicationInfo::algorithmSet()
{
	return "motion0";
}

ApplicationInfo::ApplicationInfo()
{
}

