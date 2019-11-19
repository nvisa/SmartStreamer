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

class AlarmSource
{
public:
	AlarmSource();

	bool wait(int msecs);
	virtual QString typeString() const = 0;
	QHash<QString, QVariant> fetch();
	virtual void reset();

protected:
	void push(const QHash<QString, QVariant> &h);

	int queueLen;
private:
	QMutex m;
	QWaitCondition wc;
	QList<QHash<QString, QVariant>> queue;
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
};

class MultipleAlarmSource
{
public:
	MultipleAlarmSource();

	void addSource(QSharedPointer<AlarmSource> source);
	QList<QSharedPointer<AlarmSource>> wait(int msecs = 1000);

protected:
	QList<QSharedPointer<AlarmSource>> sources;
};

#endif // ALARMSOURCE_H
