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

class MultipleAlarmSource;

class AlarmSource
{
public:
	AlarmSource();

	bool wait(int msecs);
	virtual QString typeString() const = 0;
	QHash<QString, QVariant> fetch();
	bool check();
	virtual void reset();
	void setParameters(const AlgorithmCommunication::AlarmReqAdvancedParameters &pars);

protected:
	virtual void fetching(QHash<QString, QVariant> &);
	void push(const QHash<QString, QVariant> &h);

	friend class MultipleAlarmSource;
	void addListener(MultipleAlarmSource *s);
	void removeListener(MultipleAlarmSource *s);

	AlgorithmCommunication::AlarmReqAdvancedParameters advanced;
private:
	QMutex m;
	QWaitCondition wc;
	QList<QHash<QString, QVariant>> queue;
	QList<MultipleAlarmSource *> listeners;
};

class GenericAlarmSource : public AlarmSource
{
public:
	GenericAlarmSource(const QString &type);

	QString typeString() const;
	void produce(const QHash<QString, QVariant> &h);
	void produce(const QString &key, const QVariant &value);

protected:
	QString type;
};

class MotionAlarmSource : public AlarmSource
{
public:
	MotionAlarmSource();

	virtual QString typeString() const;
	void produce(const QString &uuid, const QString &json, const QByteArray &snapshot);
	void notifyNoMotion(const QString &uuid, const QString &json, const QByteArray &snapshot);
	void reset() override;

protected:
	void fetching(QHash<QString, QVariant> &h) override;

	QString id;
	QByteArray lastSnapshot;
	QHash<QString, int> alarmCount;
	QElapsedTimer lastAlarmElapsed;
	QElapsedTimer noAlarmElapsed;
	QElapsedTimer lastSnapshotTime;
	QMutex motex;
};

class TrackAlarmSource : public AlarmSource
{
public:
	TrackAlarmSource();

	virtual QString typeString() const;
	void produce(const QString &uuid, const QString &json, const QByteArray &snapshot);
};

class MultipleAlarmSource
{
public:
	MultipleAlarmSource();
	~MultipleAlarmSource();

	void addSource(QSharedPointer<AlarmSource> source);
	QList<QSharedPointer<AlarmSource>> wait(int msecs = 1000);

protected:
	friend class AlarmSource;
	void notify();

protected:
	QList<QSharedPointer<AlarmSource>> sources;
	QWaitCondition wc;
	QMutex mutex;
};

#endif // ALARMSOURCE_H
