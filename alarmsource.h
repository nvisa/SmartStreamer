#ifndef ALARMSOURCE_H
#define ALARMSOURCE_H

#include <QHash>
#include <QList>
#include <QMutex>
#include <QString>
#include <QVariant>
#include <QByteArray>
#include <QElapsedTimer>
#include <QSharedPointer>
#include <QWaitCondition>

#include <memory>

#include "proto/AlgorithmCommunication.pb.h"
#include "proto/v2/AlgorithmCommunicationV2.pb.h"

class MultipleAlarmSource;

class AlarmSource
{
public:
	AlarmSource();

	class QueueData
	{
	public:
		QHash<QString, QVariant> hash;
		QList<algorithm::v2::DetectedObject> detectedObjects;
	};

	bool wait(int msecs);
	virtual QString typeString() const = 0;
	QueueData fetch();
	bool check();
	virtual void reset();
	void setParameters(const AlgorithmCommunication::AlarmReqAdvancedParameters &pars);
	void setParameters(const algorithm::v2::AlarmReqAdvancedParameters &pars);

protected:
	virtual void fetching(QueueData &);
	void push(const QueueData &h);
	void addSnapshotToAlarm(QueueData &h, const QString &id);

	friend class MultipleAlarmSource;
	void addListener(MultipleAlarmSource *s);
	void removeListener(MultipleAlarmSource *s);

	AlgorithmCommunication::AlarmReqAdvancedParameters advanced;
	algorithm::v2::AlarmReqAdvancedParameters advancedV2;
	QByteArray lastSnapshot;
	QHash<QString, int> alarmCount;
	QElapsedTimer lastSnapshotTime;
	QMutex motex;
private:
	QMutex m;
	QWaitCondition wc;
	QList<QueueData> queue;
	QList<MultipleAlarmSource *> listeners;
};

class GenericAlarmSource : public AlarmSource
{
public:
	GenericAlarmSource(const QString &type);

	QString typeString() const;
	void produce(const QueueData &h);
	void produce(const QString &key, const QVariant &value);

protected:
	QString type;
};

class MotionAlarmSource : public AlarmSource
{
public:
	MotionAlarmSource();

	virtual QString typeString() const;
	void produce(const QString &uuid, const QString &json, const QByteArray &snapshot, QList<algorithm::v2::DetectedObject> objects = QList<algorithm::v2::DetectedObject>());
	void notifyNoMotion(const QString &uuid, const QString &json, const QByteArray &snapshot);
	void reset() override;

protected:
	void fetching(QueueData &h) override;

	QString id;
	QElapsedTimer lastAlarmElapsed;
	QElapsedTimer noAlarmElapsed;
};

class TrackAlarmSource : public AlarmSource
{
public:
	TrackAlarmSource();

	virtual QString typeString() const;
	void produce(const QString &uuid, const QString &json, const QByteArray &snapshot);

protected:
	void fetching(QueueData &h) override;
};

class MultipleAlarmSource
{
public:
	MultipleAlarmSource();
	~MultipleAlarmSource();

	void addSource(QSharedPointer<AlarmSource> source);
	QList<QSharedPointer<AlarmSource>> wait(int msecs = 1000);
	int count();
	bool contains(const QString &type);

protected:
	friend class AlarmSource;
	void notify();

protected:
	QList<QSharedPointer<AlarmSource>> sources;
	QWaitCondition wc;
	QMutex mutex;
};

#endif // ALARMSOURCE_H
