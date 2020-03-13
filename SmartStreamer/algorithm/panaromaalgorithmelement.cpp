#include "panaromaalgorithmelement.h"

#include "lmm/debug.h"
#include "algorithmfunctions.h"
#include "unistd.h"

#include "applicationinfo.h"
#include "aryadriver.h"
#include "aryapthead.h"

#include <QProcess>
#include <QBuffer>
#include <QImage>
#include <QFile>

static int refreshPanConfigs(float fov)
{
	QFile f("/etc/smartstreamer/tk1/pan_params.txt");
	if (!f.open(QIODevice::ReadOnly))
		return -1;
	QString data = f.readAll();
	f.close();
	if (data.isEmpty())
		return -2;
	QStringList configs;
	foreach (QString line, data.split("\n")) {
		if (line.isEmpty())
			continue;
		if (line.contains(" "))
			line = line.split(" ").first();
		if (line.contains("//"))
			line.remove("//");
		configs << line;
	}
	configs.replace(1, QString("%1").arg(fov));
	if (f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
		f.write(configs.join("\n").toLatin1());
		f.close();
	}
	return 0;
}

PanaromaAlgorithmElement::PanaromaAlgorithmElement(QObject *parent)
	: BaseAlgorithmElement(parent)
{
	algoState = UNKNOWN;
	pt = nullptr;
	control.fovValue = 17.5;
	control.targetFov = 0;
}

int PanaromaAlgorithmElement::init()
{
	QProcess::execute("bash -c \"rm -f /home/ubuntu/Desktop/Pan_images/*\"");

	pt = getPanTiltHead();
	v.debug = 0;
	v.ill = 1;
	v.privacy = 0;
	v.record = 0;
	v.rgb = 1;
	v.shadow = 0;
	v.stabilization = 0;
	resetPosition();
	control.started.pan = pt->getPanAngle();
	control.started.tilt = pt->getTiltAngle();
	doPivot(0.20);
	control.init = 1;
	return BaseAlgorithmElement::init();
}

int PanaromaAlgorithmElement::reinit()
{
	if (!pt)
		pt = getPanTiltHead();
	control.panTiltZoomRead[0] = pt->getPanAngle();
	control.panTiltZoomRead[1] = pt->getTiltAngle();
	control.panTiltZoomRead[2] = 120000;
	return BaseAlgorithmElement::reinit();
}

int PanaromaAlgorithmElement::reallocate()
{
	if (control.meta[0] != 0) {
		algoState = STOPALGO;
		if (control.meta[3] == 1)
			control.started.pan = control.meta[1] + (float)(control.meta[2]) / 10.0;
		else if (control.meta[3] == 2)
			control.started.pan = -(control.meta[1] + (float)(control.meta[2]) / 10.0);
		else control.started.pan = 0;
	}
	return 0;
}

int PanaromaAlgorithmElement::processAlgo(const RawBuffer &buf)
{
	reinit();
	int width = buf.constPars()->videoWidth;
	int height = buf.constPars()->videoHeight;

#if HAVE_VIA_PANAROMA
	asel_pan((uchar *)buf.constData(), width * height, width, height,
			 v.rgb, v.record, v.shadow, v.ill, v.debug, control.meta, control.panTiltZoomRead, control.init);
#endif
	if (control.init)
		control.init = 0;
	reallocate();
	return 0;
}

int PanaromaAlgorithmElement::stopAlgo()
{
	pt->panTiltStop();
	pt->setTransportInterval(100);
	return BaseAlgorithmElement::stopAlgo();
}

int PanaromaAlgorithmElement::release()
{
#if HAVE_VIA_PANAROMA
	asel_pan_release();
#endif
	return BaseAlgorithmElement::release();
}

int PanaromaAlgorithmElement::resetPosition()
{
	if (!pt)
		pt = getPanTiltHead();
	ApplicationInfo::instance()->getPtzpDriver(0)->getHead(1)->setProperty(2, control.targetFov);
	int time = 5;
	int pan = pt->getPanAngle();
	if (pan >= 180)
		time = (360 - pan) * 0.04 + 1;
	else
		time = (pan - 0) * 0.04 + 1;
	pt->panTiltGoPos(0, 0);
	sleep(time + 15); // zoom position reset.
	int ret = refreshPanConfigs(ApplicationInfo::instance()->getPtzpDriver(0)->getHead(1)->getAngle());
	if (ret < 0)
		mDebug("Fov value can't write to 'pan_params.txt'");
	return 0;
}

int PanaromaAlgorithmElement::doPivot(float speed)
{
	if (!pt)
		pt = getPanTiltHead();
	pt->setTransportInterval(25);
	pt->panTiltAbs(speed, 0);
	return 0;
}

int PanaromaAlgorithmElement::reloadJson(const QJsonObject &node)
{
	Q_UNUSED(node);
	control.fovValue = node["fov"].toDouble();
	control.targetFov = node["target_fov"].toInt();
	return 0;
}

QJsonObject PanaromaAlgorithmElement::resaveJson(const QJsonObject &node)
{
	Q_UNUSED(node);
	return QJsonObject();
}

PtzpHead *PanaromaAlgorithmElement::getPanTiltHead()
{
	return ApplicationInfo::instance()->getPtzpDriver(0)->getHead(0);
}

QByteArray PanaromaAlgorithmElement::getPanaromaFrame(const QString &picture)
{
	QImage im(picture);
	QByteArray ba;
	QBuffer qbuf(&ba);
	qbuf.open(QIODevice::WriteOnly);
	im.save(&qbuf, "JPG");
	return ba;
}

QString PanaromaAlgorithmElement::getTypeString()
{
	return "panaroma";
}
