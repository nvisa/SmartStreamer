#include "panaromaalgorithmelement.h"

#include "lmm/debug.h"
#include "algorithmfunctions.h"
#include "unistd.h"

#include "applicationinfo.h"
#include "ecl/ptzp/aryadriver.h"
#include "ecl/ptzp/aryapthead.h"

#include <QProcess>
#include <QBuffer>
#include <QImage>

PanaromaAlgorithmElement::PanaromaAlgorithmElement(QObject *parent)
	: BaseAlgorithmElement(parent)
{
	algoState = UNKNOWN;
	pt = nullptr;
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
	doPivot(0.03);
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
	return newOutputBuffer(buf);
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
	int time = 5;
	int pan = pt->getPanAngle();
	if (pan >= 180)
		time = (360 - pan) * 0.04 + 1;
	else
		time = (pan - 0) * 0.04 + 1;
	pt->panTiltGoPos(0, 0);
	sleep(time);
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
