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
#include "moxak1streamer.h"
#include "streamercommon.h"
#include "usersettings.h"

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

#include <ecl/debug.h>
#include <ecl/ptzp/kayidriver.h>
#include <ecl/ptzp/tbgthdriver.h>
#include <ecl/ptzp/aryadriver.h>
#include <ecl/ptzp/flirdriver.h>
#include <ecl/ptzp/irdomedriver.h>
#include <ecl/ptzp/yamgozdriver.h>
#include <ecl/ptzp/swirdriver.h>
#include <ecl/ptzp/htrswirdriver.h>
#include <ecl/ptzp/oem4kdriver.h>
#include <ecl/drivers/systeminfo.h>
#include <ecl/ptzp/virtualptzpdriver.h>

#include <unistd.h>

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
	QJsonObject obj = StreamerCommon::readSettingsJSON("/etc/smartstreamer/smartconfig.json").object();
	QJsonArray arr = obj["ptzp"].toArray();
	Platform plat = getApplicationPlatform();
	foreach (QJsonValue val, arr) {
		QJsonObject obj = val.toObject();
		PtzpDriver *driver = NULL;
		switch (plat) {
		case GENERIC:
			driver = new VirtualPtzpDriver;
			break;
		case KAYI_SAHINGOZ: {
			QList<int> relayConfig;
			if (!obj["relay"].toObject().isEmpty()) {
				relayConfig << obj["relay"].toObject()["daycam"].toInt();
				relayConfig << obj["relay"].toObject()["thermal"].toInt();
				relayConfig << obj["relay"].toObject()["standby"].toInt();
			}
			driver = new KayiDriver(relayConfig, obj["gps"].toBool());
			break;
		}
		case ARYA_ORION: {
			driver = new AryaDriver;
			((AryaDriver*) driver)->setMoxaControl(obj["moxa_thermal"].toString(), obj["moxa_day"].toString());
			((AryaDriver*) driver)->setOverlayInterval(obj["overlay_interval"].toInt());
			((AryaDriver*) driver)->setThermalInterval(obj["thermal_interval"].toInt());
			((AryaDriver*) driver)->setGungorInterval(obj["gungor_interval"].toInt());
			((AryaDriver*) driver)->setHeadType(obj["head_type"].toString());
			break;
		}
		case TBGTH:
			if (!obj["thermal"].toBool())
				driver = new TbgthDriver(false);
			else
				driver = new TbgthDriver(true);
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
			break;
		case HTR_SWIR:
			driver = new HtrSwirDriver;
			break;
		case OEM4K:
			driver = new Oem4kDriver;
			break;
		}
		if (driver) {
			fDebug("Starting PTZP driver for %s", qPrintable(obj["type"].toString()));
			driver->setRegisterSaving(obj["save_registers"].toBool(), obj["save_register_interval"].toInt());
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
	QJsonObject obj = StreamerCommon::readSettingsJSON("/etc/smartstreamer/smartconfig.json").object();
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
		} else if (obj["type"] == QString("htrswir")) {
			return HTR_SWIR;
		} else if (obj["type"] == QString("oem4k")) {
			return OEM4K;
		}

	}
	return GENERIC;
}

QString ApplicationInfo::getAselsanPlatform()
{
	/*
	 * TODO: We need to sync this info with getApplicationPlatform().
	 * Moreover, we need to get rid of from one.
	 */
	QFile f("/etc/aselsan_platform");
	f.open(QIODevice::ReadOnly);
	QString plat = QString::fromUtf8(f.readAll()).trimmed();
	f.close();
	return plat;
}

PtzpDriver *ApplicationInfo::getPtzpDriver(int index)
{
	if (index < drivers.size())
		return drivers[index];
	return nullptr;
}

QJsonObject ApplicationInfo::getPtzpObject(int index)
{
	QJsonObject obj = StreamerCommon::readSettingsJSON("/etc/smartstreamer/smartconfig.json").object();
	QJsonArray arr = obj["ptzp"].toArray();
	if (index < arr.size())
		return arr[index].toObject();
	return QJsonObject();
}

BaseStreamer *ApplicationInfo::createAppStreamer()
{
	BaseStreamer *streamer = nullptr;
	QJsonObject obj = StreamerCommon::readSettingsJSON("/etc/smartstreamer/smartconfig.json").object();
#if HAVE_TX1
	if (obj.value("ipstreamer").toBool()) {
		qDebug() << "starting ip stramer";
		IpStreamer *ipStr = new IpStreamer(obj["ipstreamer_config"].toObject());
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
	} else if (obj.value("moxak1streamer").toBool()) {
		qDebug() << "Moxa driver is initializing";
		streamer = new MoxaK1Streamer(obj["moxak1_config"].toObject());
	}
#endif
	ffDebug() << "Starting application under path" << QDir::currentPath();
	return streamer;
}

#if HAVE_TK1
BaseAlgorithmElement *ApplicationInfo::createAlgorithmFromJson(const QJsonObject &algo)
{
	if (algo["type"] == QString("motion")) {
		return new MotionAlgorithmElement;
	} else if (algo["type"] == QString("panaroma")) {
		return new PanaromaAlgorithmElement;
	}
	return new BaseAlgorithmElement;
}
#endif

#if HAVE_TX1
BaseAlgorithmElement *ApplicationInfo::createAlgorithmFromJson(const QJsonObject &algo)
{
	if (algo["type"] == QString("motion")) {
		return new MotionAlgorithmElement;
	} else if (algo["type"] == QString("bypass")) {
		return new StabilizationAlgorithmElement;
	} else if (algo["type"] == QString("privacy")) {
		return new StabilizationAlgorithmElement;
	} else if (algo["type"] == QString("track")) {
		TrackAlgorithmElement *track = new TrackAlgorithmElement;
		if (getApplicationPlatform() == KAYI_SAHINGOZ)
			track->setTiltReversing(true);
		return track;
	} else if (algo["type"] == QString("faceDetection")) {
		return new FaceAlgorithmElement;
	} else if (algo["type"] == QString("panchange")) {
		return new PanChangeAlgorithmElement;
	}
	return new BaseAlgorithmElement;
}
#endif

BaseAlgorithmElement *ApplicationInfo::createAlgorithm(const QString &type, int index)
{
	QJsonObject obj = StreamerCommon::readSettingsJSON("/etc/smartstreamer/algodesc.json").object();
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
	BaseAlgorithmElement *el = nullptr;
	if (filtered.size() <= index)
		el = new BaseAlgorithmElement;
	else {
		QJsonObject algo = filtered[index].toObject();
		el = createAlgorithmFromJson(algo);
		el->setJsonAlgorithmIndex(imap[index]);
		el->reloadJson();
	}

	algorithms << el;
	algoNameMapping[type] << el;
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
	QJsonObject obj = StreamerCommon::readSettingsJSON("/etc/smartstreamer/smartconfig.json").object();
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

int ApplicationInfo::init()
{
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), SLOT(timeoutApp()));
	timer->start(1000);
	return 0;
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

BaseAlgorithmElement *ApplicationInfo::getAlgorithmInstance(int index)
{
	if (index < algorithms.size())
		return algorithms[index];
	return NULL;
}

BaseAlgorithmElement *ApplicationInfo::getAlgorithmInstance(const QString &type, int index)
{
	return algoNameMapping[type][index];
}

qint64 ApplicationInfo::getLifeTime()
{
	return lifetime->saveInterval(1000);
}

ApplicationInfo::ApplicationInfo()
{
	UserSettings::instance().setPlatform(getAselsanPlatform());
	lifetime = new LifeTimeTracker("/etc/system.lifetime");
	idt = nullptr;
}

void ApplicationInfo::timeoutApp()
{
	lifetime->saveInterval();
}
