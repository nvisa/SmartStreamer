#include "seiinserter.h"

//#include <gpiocontroller.h>

#include <lmm/debug.h>
//#include <lmm/dmai/h264encoder.h>

#include <QFile>
#include <QDateTime>

#include <errno.h>

static inline int getInt32LE(const char *ba, int &ind)
{
	ind = ind + 4;
	return	((const unsigned char *)(ba))[0] + (((const unsigned char * )(ba))[1] << 8) + (((const unsigned char *)(ba))[2] << 16) + (((const unsigned char *)(ba))[3] << 24);

}
static inline unsigned char getUInt8LE(const char *ba, int &ind)
{
	ind = ind + 1;
	return	((const unsigned char *)(ba))[0];

}
static inline quint16 getUInt16LE(const char *ba, int &ind)
{
	ind = ind + 2;
	return	((const unsigned char *)(ba))[0] + (((const unsigned char * )(ba))[1] << 8);

}

#define getInt32LE2(_x) ((const unsigned char *)(_x))[0] + (((const unsigned char * )(_x))[1] << 8) + (((const unsigned char *)(_x))[2] << 16) + (((const unsigned char *)(_x))[3] << 24)

seiXmlData::seiXmlData() :
	bTrackMode(0),
	panDeg(0),
	tiltDeg(0),
	sType(MUNDEFINED),
	nText(0),
	nLine(0),
	nRect(0),
	nPoly(0),
	nCircle(0)
{
}

SeiInserter::SeiInserter(QObject *parent) :
	BaseLmmElement(parent),
	alarmID(1),
	insertSeiAlarm(false)
{
	seiAlarm = (AlarmProps){ALARM_IDLE, 0 ,300, 3000, 5000};
	seiActDur = 200;
	seiTimerStarted = false;
	motprov = NULL;
	//	gpio = NULL;
	qsrand(uint(QDateTime::currentDateTime().toTime_t() & 0xFFFF));
	seiXml = new seiXmlData();
}

void SeiInserter::setMotionDetectionProvider(MotionDetectionInterface *iface)
{
	motprov = iface;
}

int SeiInserter::setAlarmTemplate(const QString &filename)
{
	QFile f(filename);
	if (!f.open(QIODevice::ReadOnly))
		return -ENOENT;
	info.alarmTemplate = QString::fromUtf8(f.readAll());
	f.close();
	return 0;
}

int SeiInserter::setSeiProps(SeiInserter::AlarmType type, const QByteArray &data)
{
	lock.lock();
	info.algorithm = 0;
	incomingAlarmType = type;
	incomingSeiData = data;
	isSeiNew = true;
	lock.unlock();
	return 0;
}

int SeiInserter::addGpioAlarm(int io)
{
	//	if (!gpio)
	//		gpio = new GpioController(this);
	//	int err = gpio->requestGpio(io, GpioController::INPUT);
	//	if (err)
	//		return err;
	if (!info.ios.contains(io))
		info.ios << io;
	return 0;
}

int SeiInserter::setAlarmInformation(const QString &templateFile, int alg, int gpioId, int ioLevel, int minDuration, int motThreshold)
{
	int err = setAlarmTemplate(templateFile);
	if (err)
		return err;
	info.alarmTemplateFilename = templateFile;
	info.algorithm = alg;
	info.enabled = true;
	info.ioAlarmLevel = ioLevel;
	info.minAlarmDuration = minDuration;
	info.motionAlarmThreshold = motThreshold * NORMALIZE_MOTION;
	info.motionAlarmThrPercent = motThreshold;

	if (gpioId >= 0)
		addGpioAlarm(gpioId);
	return 0;
}

const QByteArray SeiInserter::generateAlarm()
{
	QByteArray ba;
	int activeio;

	/* we will generate our own alarms */
	info.algorithm = 1;
	if (info.algorithm == 1) {
		int alarmActive = 0;
		int alarmSource = 0;
		int motv = 0;/*motprov->getMotionValue();*/
		if (motv == 0)
			/* this one is I-frame */
			motv = info.lastMotionValue;
		info.lastMotionValue = motv;
		int ioState = 0;
		/* we shouldn't generate alarms more often than minAlarmDuration*/
		bool canTrigger = true;
		if (info.current.active && info.current.t.elapsed() < info.minAlarmDuration)
			canTrigger = false;
		if (canTrigger) {
			/* let's first check motion alarm */
			if (motv > info.motionAlarmThreshold) {
				alarmActive = 1;
				alarmSource = info.current.source | 0x1;
				if (info.current.active == false
						|| info.current.source != alarmSource) {
					/* we should generate a new alarm */
					info.current.triggerNew(alarmSource);
				}
#if 0
				else if (info.current.t.elapsed() > info.minAlarmDuration) {
					/* we should trigger a new alarm */
					info.current.triggerNew(alarmSource);
				}
#endif
			} else if (info.current.active && info.current.source != 2) {
				/* motion alarm should suppress */
				info.current.suppress();
				alarmSource = 0;
			}
			/* then we check IO alarms */
			foreach (int io, info.ios) {
				int val = 0; /*gpio->getGpioValue(io);*/
				if (info.ioAlarmLevel == val) {
					activeio = io;
					alarmActive = 1;
					alarmSource = alarmSource | 0x2;
					if (info.current.active == false
							|| info.current.source != alarmSource)
						info.current.triggerNew(alarmSource);
#if 0
					else if (info.current.t.elapsed() > info.minAlarmDuration)
						info.current.triggerNew(alarmSource);
#endif
				} else {
					info.current.source = alarmSource & ~0x02;
					if (info.current.source == 0)
						info.current.suppress();
				}
				ioState = val;
			}
		}
		info.current.active = 1;
		if (!info.current.active)
			return 0;

		motv /= NORMALIZE_MOTION;
		if (motv > 100)
			motv = 100;

		bool ok;
		ba = info.alarmTemplate
				/*
								 * We have N fields in our template file:
								 *  1st: alarm status, 0: inactive, 1: active
								 *	2nd: alarm ID if alarm is present
								 *  3rd: alarm source if alarm is present
								 *  4th: absolute motion value
								 *  5th: motion detection threshold
								 *  6th: IO-1 output state
								 */
				.arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz"))
				.arg(info.current.id.toLongLong(&ok, 16))
				.arg(info.current.source)
				.arg(motv)
				.arg(info.motionAlarmThrPercent)
				.arg(info.ios.at(0))
				.arg(info.current.id.toLongLong(&ok, 16))
				.arg(QDateTime::currentDateTime().toString("yyyyMMdd_hh:mm:ss:zzz"))
				.arg(info.current.source)
				.arg(motv)
				.arg(info.motionAlarmThrPercent)
				.arg(info.ios.at(0))

				.toUtf8();

		//qDebug() << QString::fromUtf8(ba);
	}

	return ba;
}

void SeiInserter::setSeiField(RawBuffer buf)
{
	buf.pars()->metaData.clear();
	QByteArray seiData = incomingSeiData;
	if (isSeiNew == true) {
		isSeiNew = false;
		seiAlarm.alarmType = incomingAlarmType;
		insertSeiAlarm = true;
		if (seiAlarm.alarmType == ALARM_POST)
			seiTimerStarted = false;
	}
	if (seiAlarm.alarmType == ALARM_ONLINE) {
		buf.pars()->metaData = seiData;
	} else if (seiAlarm.alarmType == ALARM_STOP) {
		insertSeiAlarm = false;
		buf.pars()->metaData = seiData;
		seiAlarm.alarmType = ALARM_IDLE;
	} else if (seiAlarm.alarmType == ALARM_POST) {
		if (insertSeiAlarm == true) {
			if (seiTimerStarted == false) {
				seiTimer.start();
				seiTimerStarted = true;
			}
			buf.pars()->metaData = seiData;
		}
	}
}

int SeiInserter::processBuffer(const RawBuffer &buf)
{
	/* if we have libsmart meta, do not mess with sei */
	if (buf.constPars()->metaDataRaw)
		return 0;

	lock.lock();

	/* first check alarm duration */
	if (seiAlarm.alarmType == ALARM_POST && seiTimer.elapsed() > seiActDur) {
		insertSeiAlarm = false;
		seiAlarm.alarmType = ALARM_STOP;
		incomingSeiData.clear();
	}

	/* now insert sei data */
	info.algorithm = 0;
	if (info.algorithm == 0)
		setSeiField(buf);
	else if (info.algorithm == 1)
		((RawBuffer *)&buf)->pars()->metaData = generateAlarm();

	/* we are done */
	lock.unlock();

	return newOutputBuffer(0, buf);
}

void SeiInserter::setAlarmID()
{
	seiAlarm.alarmNumber = alarmID;
	alarmID++;
}

void SeiInserter::processMessage(QByteArray datagram)
{
	int ind = 0;
	int datasize = getUInt8LE(datagram.constData() + ind, ind);
	if (datasize <= 2) {
		return;
	}
	int trackmode = getUInt8LE(datagram.constData() + ind, ind);
	if (!trackmode) {
		seiXml->sType = (seiXmlData::seiType2)(getUInt8LE(datagram.constData() + ind, ind));
		seiXml->nText = getUInt8LE(datagram.constData() + ind, ind);
		seiXml->nLine = getUInt8LE(datagram.constData() + ind, ind);
		seiXml->nRect = getUInt8LE(datagram.constData() + ind, ind);
		seiXml->nPoly = getUInt8LE(datagram.constData() + ind, ind);
		seiXml->nCircle = getUInt8LE(datagram.constData() + ind, ind);

		QByteArray newData = datagram.remove(0, ind);
		QByteArray out;
		out = createSEI(seiXml, newData);
		setSeiProps(ALARM_ONLINE, out);

		//qDebug() << "seiXml->sType: " << seiXml->sType << "seiXml->nText: " << seiXml->nText << "seiXml->nLine: " << seiXml->nLine << "seiXml->nRect: " << seiXml->nRect << "seiXml->nPoly: " << seiXml->nPoly << "seiXml->nCircle: " << seiXml->nCircle << "ind: " << ind;
	} else {
		setSeiProps(ALARM_IDLE, 0);
	}
}

QByteArray SeiInserter::createSEI(seiXmlData *xmlData, QByteArray in)
{
	QString templ = "<?xml version='1.0' encoding='UTF-8'?><SEI><TI>%1</TI><OI>%2%3%4%5%6</OI>%7</SEI>";
	QString textSei;
	QString lineSei;
	QString rectSei;
	QString polySei;
	QString circleSei;
	QString alarmSei;
	static int alarmFlag = 0;
	int alarmSource = 1;
	bool ok;

	if (t2 < t1)
		t2+= 24 * 3600;

	if (tnow > t1 && tnow < t2)
		allowAlarm = true;
	else if ((tnow + 24 * 3600) > t1 && (tnow + 24 * 3600) < t2)
		allowAlarm = true;
	else
		allowAlarm = false;

	allowAlarm = true;
	if (allowAlarm) {
		if (xmlData->nRect == 0) {
			if (alarmFlag) {
				if (linkedAlarms.elapsed() > 5000) {
					info.current.triggerNew(alarmSource);
					qDebug() << "alarm triggered at: " << tnow << "duration: " << alarmTimer.elapsed();
					alarmSei.append(QString("<AI N=\"%1\" D=\"%2\">")
									.arg(info.current.id.toLongLong(&ok, 16)).arg(alarmTimer.elapsed()));
					alarmSei.append(QString("<ADI K=\"1\" V=\"%1\"/>")
									.arg(info.current.source));
					alarmSei.append(QString("</AI>"));
					alarmFlag = 0;
				}
			} else {
				info.current.suppress();
				alarmTimer.start();
			}
		} else {
			alarmFlag = 1;
			linkedAlarms.restart();
		}
	}

	int ind = 0;
	for (int i = 0; i < xmlData->nText; i++) {
		quint16 pcnt1 = getUInt16LE(in.constData() + ind, ind);
		quint16 pcnt2 = getUInt16LE(in.constData() + ind, ind);
		quint8 clr1 = getUInt8LE(in.constData() + ind, ind);
		quint8 clr2 = getUInt8LE(in.constData() + ind, ind);
		quint8 clr3 = getUInt8LE(in.constData() + ind, ind);
		int dataType = getUInt8LE(in.constData() + ind, ind);
		int x;
		if(dataType == 0)
			x = getUInt16LE(in.constData() + ind, ind);
		else
			x = getUInt8LE(in.constData() + ind, ind);
		textSei.append(QString("<TI PNT=\"%1,%2\" CLR=\"%3,%4,%5\" T=\"%6\"/>")
					   .arg(pcnt1)
					   .arg(pcnt2)
					   .arg(clr1)
					   .arg(clr2)
					   .arg(clr3)
					   .arg(x)
					   );
	}
	for (int i = 0; i < xmlData->nLine; i++) {
		quint16 startX = getUInt16LE(in.constData() + ind, ind);
		quint16 startY = getUInt16LE(in.constData() + ind, ind);
		quint16 endX = getUInt16LE(in.constData() + ind, ind);
		quint16 endY = getUInt16LE(in.constData() + ind, ind);
		quint8 clr1 = getUInt8LE(in.constData() + ind, ind);
		quint8 clr2 = getUInt8LE(in.constData() + ind, ind);
		quint8 clr3 = getUInt8LE(in.constData() + ind, ind);
		lineSei.append(QString("<LI S=\"%1,%2\" E=\"%3,%4\" C=\"%5,%6,%7\"/>")
					   .arg(startX)
					   .arg(startY)
					   .arg(endX)
					   .arg(endY)
					   .arg(clr1)
					   .arg(clr2)
					   .arg(clr3)
					   );
	}
	for (int i = 0; i < xmlData->nRect; i++) {
		quint16 c1x = getUInt16LE(in.constData() + ind, ind);
		quint16 c1y = getUInt16LE(in.constData() + ind, ind);
		quint16 c2x = getUInt16LE(in.constData() + ind, ind);
		quint16 c2y = getUInt16LE(in.constData() + ind, ind);
		quint8 clr1 = getUInt8LE(in.constData() + ind, ind);
		quint8 clr2 = getUInt8LE(in.constData() + ind, ind);
		quint8 clr3 = getUInt8LE(in.constData() + ind, ind);
		quint8 fill = getUInt8LE(in.constData() + ind, ind);
		rectSei.append(QString("<RI C1=\"%1,%2\" C2=\"%3,%4\" CLR=\"%5,%6,%7\" F=\"%8\"/>")
					   .arg(c1x)
					   .arg(c1y)
					   .arg(c2x)
					   .arg(c2y)
					   .arg(clr1)
					   .arg(clr2)
					   .arg(clr3)
					   .arg(fill)
					   );
	}
	for (int i = 0; i < xmlData->nPoly; i++) {
		QString polyPoints;
		quint8 nPoint = getUInt8LE(in.constData() + ind, ind);
		for (int i = 0; i < nPoint; i++) {
			quint16 polyx = getUInt16LE(in.constData() + ind, ind);
			quint16 polyy = getUInt16LE(in.constData() + ind, ind);
			polyPoints.append(QString("<P PNT=\"%1,%2\"/>")
							  .arg(polyx)
							  .arg(polyy)
							  );
		}
		quint8 clr1 = getUInt8LE(in.constData() + ind, ind);
		quint8 clr2 = getUInt8LE(in.constData() + ind, ind);
		quint8 clr3 = getUInt8LE(in.constData() + ind, ind);
		quint8 fill = getUInt8LE(in.constData() + ind, ind);
		polySei.append(QString("<PI C=\"%1,%2,%3\" F=\"%4\">")
					   .arg(clr1)
					   .arg(clr2)
					   .arg(clr3)
					   .arg(fill)
					   );
		polySei.append(polyPoints);
		polySei.append(QString("</PI>"));
	}
	for (int i = 0; i < xmlData->nCircle; i++) {
		quint16 pnt1 = getUInt16LE(in.constData() + ind, ind);
		quint16 pnt2 = getUInt16LE(in.constData() + ind, ind);
		quint8 clr1 = getUInt8LE(in.constData() + ind, ind);
		quint8 clr2 = getUInt8LE(in.constData() + ind, ind);
		quint8 clr3 = getUInt8LE(in.constData() + ind, ind);
		circleSei.append(QString("<CI PNT=\"%1,%2\" CLR=\"%3,%4,%5\"/>")
						 .arg(pnt1)
						 .arg(pnt2)
						 .arg(clr1)
						 .arg(clr2)
						 .arg(clr3)
						 );
	}

	return templ.arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz")).arg(textSei).arg(lineSei).arg(rectSei).arg(polySei).arg(circleSei).arg(alarmSei).toUtf8();
}

void SeiInserter::clearLastSEIMessage()
{
	seiAlarm.alarmType = ALARM_IDLE;
	incomingSeiData.clear();
}
