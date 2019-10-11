#include "indevicetest.h"

#include <ecl/debug.h>
#include <ecl/ptzp/ptzphead.h>
#include <ecl/ptzp/ptzpdriver.h>

#include <lmm/baselmmpipeline.h>

#include <QTimer>
#include <QJsonArray>
#include <QJsonDocument>

InDeviceTest::InDeviceTest(bool enableOnline, QObject *parent)
	: QObject(parent),
	  online(enableOnline)
{
	checkTime = 10000;
	headPingFaultTime = 3000;
	pipelineFaultTime = 10000;
	if (online) {
		QTimer *timer = new QTimer(this);
		timer->start(checkTime);
		connect(timer, SIGNAL(timeout()), SLOT(timeout()));
	}
}

void InDeviceTest::addPtzpDriver(const QString &name, PtzpDriver *driver)
{
	drivers.insert(name, driver);
}

void InDeviceTest::addPipeline(const QString &name, BaseLmmPipeline *p)
{
	pipelines.insert(name, p);
}

QJsonObject InDeviceTest::getLastCheckResults()
{
	QMutexLocker ml(&mutex);
	return lastPeriodicCheck;
}

void InDeviceTest::timeout()
{
	QJsonArray checks;
	foreach (const QString &key, drivers.keys())
		checks.append(QJsonValue(key));
	foreach (const QString &key, pipelines.keys())
		checks.append(QJsonValue(key));
	QJsonArray faults;
	QJsonArray extras;
	/* check drivers */
	QHashIterator<QString, PtzpDriver *> dit(drivers);
	while (dit.hasNext()) {
		dit.next();
		mInfo("Checking PTZP driver %s", qPrintable(dit.key()));
		PtzpDriver *d = dit.value();
		for (int i = 0; i < d->getHeadCount(); i++) {
			int lastPing = d->getHead(i)->communicationElapsed();
			mInfo("Checking head %d (%d)", i, lastPing);
			if (lastPing > headPingFaultTime)
				faults.append(addHeadFault(d->getHead(i)->getHeadName(), i, lastPing));
		}
		QJsonObject extra = d->doExtraDeviceTests();
		if (!extra.isEmpty())
			extras.append(extra);
	}

	/* check pipelines */
	QHashIterator<QString, BaseLmmPipeline *> pit(pipelines);
	while (pit.hasNext()) {
		pit.next();
		BaseLmmPipeline *p = pit.value();
		qint64 ping = p->getOutputQueue(0)->getElapsedSinceLastAdd();
		mInfo("Checking pipeline %s (%lld)", qPrintable(pit.key()), ping);
		if (ping > pipelineFaultTime)
			faults.append(addPipelineFault(pit.key(), ping));
	}

	QJsonObject obj;
	obj.insert("device_checks", checks);
	if (!extras.isEmpty())
		obj.insert("extras", extras);
	if (faults.size())
		obj.insert("faults", faults);
	mutex.lock();
	lastPeriodicCheck = obj;
	mutex.unlock();
}

QJsonObject InDeviceTest::addHeadFault(const QString &name, int head, int elapsed)
{
	QJsonObject obj;
	obj.insert("type", QString("control_module"));
	obj.insert("index", head);
	obj.insert("name", name);
	obj.insert("elapsed_since_last_valid", elapsed);
	return obj;
}

QJsonObject InDeviceTest::addPipelineFault(const QString &name, qint64 elapsed)
{
	QJsonObject obj;
	obj.insert("type", QString("video_module"));
	obj.insert("name", name);
	obj.insert("elapsed_since_last_valid", elapsed);
	return obj;
}

