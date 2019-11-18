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
	QHash<QString, QVariant> fields();
	void clearFields();
	virtual void reset();

protected:
	void notify();
	void setFields(const QHash<QString, QVariant> &h);

private:
	QMutex m;
	QWaitCondition wc;
	QHash<QString, QVariant> fieldsHash;
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
