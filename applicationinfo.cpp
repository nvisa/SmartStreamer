#include "applicationinfo.h"
#include "yamgozstreamer.h"
#include "analogstreamer.h"
#include "ipstreamer.h"
#include "usbstreamer.h"
#include "aryastreamer.h"
#include "flirstreamer.h"
#include "indevicetest.h"
#include "tbgthstreamer.h"
#include "videotestsourcestreamer.h"

#include "algorithm/motionalgorithmelement.h"
#include "algorithm/stabilizationalgorithmelement.h"
#include "algorithm/trackalgorithmelement.h"
#include "algorithm/panaromaalgorithmelement.h"
#include "kardelenapi.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QDir>

#include <ecl/ptzp/kayidriver.h>
#include <ecl/ptzp/tbgthdriver.h>
#include <ecl/ptzp/aryadriver.h>
#include <ecl/ptzp/flirdriver.h>
#include <ecl/ptzp/irdomedriver.h>
#include <ecl/ptzp/yamgozdriver.h>
#include <ecl/ptzp/swirdriver.h>
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

class LifeTimeTracker
{
public:
	LifeTimeTracker(const QString &filename)
	{
		snapshot = 0;
		snapshotFilename = filename;
		QFile f(filename);
		if (f.open(QIODevice::ReadOnly))
			snapshot = QString::fromUtf8(f.readAll()).toLongLong();
		elapsed.start();
		saveTimer.restart();
	}

	qint64 lifetime()
	{
		return snapshot + elapsed.elapsed() / 1000;
	}

	qint64 save()
	{
		QFile f(snapshotFilename);
		f.open(QIODevice::WriteOnly);
		snapshot = lifetime();
		f.write(QString::number(snapshot).toUtf8());
		f.close();
		elapsed.restart();
		return lifetime();
	}

	qint64 saveInterval(qint64 interval = 60000)
	{
		if (saveTimer.elapsed() < interval)
			return lifetime();
		save();
		saveTimer.restart();
		return lifetime();
	}

protected:

	QString snapshotFilename;
	long long snapshot;
	QElapsedTimer saveTimer;
	QElapsedTimer elapsed;
};

bool ApplicationInfo::isGuiApplication()
{
	return true;
}

int ApplicationInfo::startPtzpDriver()
{
	/* PTZP driver management */
	QJsonObject obj = readJson("/etc/smartstreamer/smartconfig.json");
	QJsonArray arr = obj["ptzp"].toArray();
	Platform plat = getApplicationPlatform();
	foreach (QJsonValue val, arr) {
		QJsonObject obj = val.toObject();
		PtzpDriver *driver = NULL;
		switch (plat) {
		case GENERIC:
			break;
		case KAYI_SAHINGOZ: {
			QList<int> relayConfig;
			if (!obj["relay"].toObject().isEmpty()) {
				relayConfig << obj["relay"].toObject()["daycam"].toInt();
				relayConfig << obj["relay"].toObject()["thermal"].toInt();
				relayConfig << obj["relay"].toObject()["standby"].toInt();
			}
			driver = new KayiDriver(relayConfig);
			break;
		}
		case ARYA_ORION:
			driver = new AryaDriver;
			break;
		case TBGTH:
			driver = new TbgthDriver(false);
			break;
		case YAMGOZ:
			driver = new YamGozDriver;
			break;
		case BOTAS_DOME:
		case BOTAS_FIX:
			driver = new IRDomeDriver;
			break;
		case FLIR_ORION:
			driver = new FlirDriver;
			break;
		case MGEO_SWIR:
			driver = new SwirDriver;
		}
		if (driver) {
			fDebug("Starting PTZP driver for %s", qPrintable(obj["type"].toString()));
			driver->setTarget(obj["target"].toString());
			if (obj.contains("grpc_port")) {
				driver->startGrpcApi(obj["grpc_port"].toInt());
				QProcess::execute("killall grpcwebproxy");
			}
			drivers << driver;
			if (obj["kardelen"].toBool())
				new KardelenAPIServer(driver, obj["type"].toString());
			idt = new InDeviceTest(true);
			idt->addPtzpDriver("ptz_controller", driver);
		}
		/* we only support one ptzp driver at the moment */
		break;
	}

	return 0;
}

ApplicationInfo::Platform ApplicationInfo::getApplicationPlatform()
{
	QJsonObject obj = readJson("/etc/smartstreamer/smartconfig.json");
	QJsonArray arr = obj["ptzp"].toArray();
	foreach (QJsonValue val, arr) {
		QJsonObject obj = val.toObject();
		if (obj["type"] == QString("kayi")) {
			return KAYI_SAHINGOZ;
		} else if (obj["type"] == QString("tbgth")) {
			return TBGTH;
		} else if (obj["type"] == QString("arya")) {
			return ARYA_ORION;
		} else if (obj["type"] == QString("irdome")) {
			return BOTAS_FIX;
		} else if (obj["type"] == QString("yamgoz")) {
			return YAMGOZ;
		} else if (obj["type"] == QString("flir")) {
			return FLIR_ORION;
		} else if (obj["type"] == QString("mgeoswir")) {
			return MGEO_SWIR;
		}
	}
	return GENERIC;
}

PtzpDriver *ApplicationInfo::getPtzpDriver(int index)
{
	if (index < drivers.size())
		return drivers[index];
	return nullptr;
}

QJsonObject ApplicationInfo::getPtzpObject(int index)
{
	QJsonObject obj = readJson("/etc/smartstreamer/smartconfig.json");
	QJsonArray arr = obj["ptzp"].toArray();
	if (index < arr.size())
		return arr[index].toObject();
	return QJsonObject();
}

BaseStreamer *ApplicationInfo::createAppStreamer()
{
	BaseStreamer *streamer = nullptr;
	QJsonObject obj = readJson("/etc/smartstreamer/smartconfig.json");
#if HAVE_TX1
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
	} else if (obj.value("tbgthstreamer").toBool()) {
		streamer = new Tbgthstreamer(obj["tbgth_config"].toObject());
	} else if (obj.value("teststreamer").toBool()) {
		streamer = new VideoTestSourceStreamer(obj["test_config"].toObject());
	} else {
		qDebug() << "starting usb streamer";
		UsbStreamer *usbStr = new UsbStreamer;
		streamer = usbStr;
	}
#elif HAVE_TK1
	if (obj.value("aryastreamer").toBool()) {
		qDebug() << "starting arya stramer";
		AryaStreamer *aryaStr = new AryaStreamer(obj["arya_config"].toObject());
		streamer = aryaStr;
	} else if (obj.value("flirstreamer").toBool()) {
		qDebug() << "starting flir streamer";
		FlirStreamer *flirStr = new FlirStreamer(obj["flir_config"].toObject());
		streamer = flirStr;
	}
#endif
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
	if (algo["type"] == QString("motion")) {
		return new MotionAlgorithmElement;
#if HAVE_TX1
	} else if (algo["type"] == QString("bypass")) {
		return new StabilizationAlgorithmElement;
	} else if (algo["type"] == QString("privacy")) {
		return new StabilizationAlgorithmElement;
	} else if (algo["type"] == QString("track")) {
		return new TrackAlgorithmElement;
	} else if (algo["type"] == QString("faceDetection")) {
		return new FaceAlgorithmElement;
#endif
#if HAVE_TK1
	} else if (algo["type"] == QString("panaroma")) {
		return new PanaromaAlgorithmElement;
#endif
	} else if (algo["type"] == QString("panchange")) {
		return new PanChangeAlgorithmElement;
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
		filtered.append(alg);
		imap.insert(filtered.size() - 1, i - 1);
	}
	if (filtered.size() <= index)
		return new BaseAlgorithmElement;

	QJsonObject algo = filtered[index].toObject();
	BaseAlgorithmElement *el = createAlgorithmFromJson(algo);
	el->setJsonAlgorithmIndex(imap[index]);
	el->reloadJson();
	algoIndexes[imap[index]] = el;
	return el;
}

void ApplicationInfo::checkStartupDelay()
{
	if (getApplicationPlatform() == KAYI_SAHINGOZ) {
		QList<int> relayConfig;
		QJsonObject obj = getPtzpObject();
		if (!obj["relay"].toObject().isEmpty()) {
			relayConfig << obj["relay"].toObject()["daycam"].toInt();
			relayConfig << obj["relay"].toObject()["thermal"].toInt();
			relayConfig << obj["relay"].toObject()["standby"].toInt();
		} else {
			relayConfig << 4;
			relayConfig << 5;
			relayConfig << 6;
		}
		QProcess::execute("i2cset -f -y 1 0x70 0x03 0x00");
		//QProcess::execute(QString("i2cset -f -y 1 0x70 0x01 0x%1").arg((1 << (relayConfig[0] - 1)), 2, 16));

	}
	QJsonObject obj = readJson("/etc/smartstreamer/smartconfig.json");
	if (!obj.contains("startup_delay"))
		return;
	int sec = obj["startup_delay"].toInt() - SystemInfo::getUptime();
	if (sec > 0) {
		qDebug("waiting application start-up delay, sleeping %d msecs", sec);
		sleep(sec);
	}
}

InDeviceTest *ApplicationInfo::getIDT()
{
	return idt;
}

QString ApplicationInfo::algorithmSet()
{
#if HAVE_TX1
	return "motion0";
#endif
#if HAVE_TK1
	return "tk1";
#endif
}

qint64 ApplicationInfo::getLifeTime()
{
	return lifetime->saveInterval(60000);
}

ApplicationInfo::ApplicationInfo()
{
	lifetime = new LifeTimeTracker("system.lifetime");
	idt = nullptr;
}
