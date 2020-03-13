#ifndef INDEVICETEST_H
#define INDEVICETEST_H

#include <QHash>
#include <QMutex>
#include <QObject>
#include <QJsonObject>

class PtzpDriver;
class BaseLmmPipeline;

class InDeviceTest : public QObject
{
	Q_OBJECT
public:
	InDeviceTest(bool enableOnline, QObject *parent = 0);

	void addPtzpDriver(const QString &name, PtzpDriver *driver);
	void addPipeline(const QString &name, BaseLmmPipeline *p);
	QJsonObject getLastCheckResults();

protected slots:
	void timeout();

protected:
	QJsonObject addHeadFault(const QString &name, int head, int elapsed);
	QJsonObject addPipelineFault(const QString &name, qint64 elapsed);
protected:
	bool online;
	int checkTime;
	int headPingFaultTime;
	int pipelineFaultTime;
	QHash<QString, PtzpDriver *> drivers;
	QHash<QString, BaseLmmPipeline *> pipelines;
	QJsonObject lastPeriodicCheck;
	QMutex mutex;
};

#endif // INDEVICETEST_H
