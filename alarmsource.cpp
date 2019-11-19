#include "alarmsource.h"

#include <QUuid>
#include <QDebug>

#if 0
#define lDebug qDebug
#else
#define lDebug(mes, arg...) do { if (0) qDebug(mes, ##arg); } while (0)
#endif

AlarmSource::AlarmSource()
{
	queueLen = 1;
}

bool AlarmSource::wait(int msecs)
{
	m.lock();
	bool v = wc.wait(&m, msecs);
	m.unlock();
	return v;
}

QHash<QString, QVariant> AlarmSource::fetch()
{
	QMutexLocker ml(&m);
	if (queue.size())
		return queue.takeFirst();
	return QHash<QString, QVariant>();
}

void AlarmSource::reset()
{
	QMutexLocker ml(&m);
	queue.clear();
}

void AlarmSource::push(const QHash<QString, QVariant> &h)
{
	QMutexLocker ml(&m);
	queue << h;
	if (queue.size() > queueLen)
		queue.removeFirst();
	wc.wakeAll();
}

MotionAlarmSource::MotionAlarmSource()
	: AlarmSource()
{
	lastAlarmElapsed.restart();
	noAlarmElapsed.restart();
}

QString MotionAlarmSource::typeString() const
{
	return "motion_detection";
}

void MotionAlarmSource::produce(const QString &uuid, const QString &json, const QByteArray &snapshot)
{
	lDebug("producing motion event");
	QHash<QString, QVariant> h;
	bool first = false;
	bool last = false;
	if (uuid.isEmpty()) {
		if (id.isEmpty()) {
			first = true;
			id = QUuid::createUuid().toString();
		}
		if (json.isEmpty())
			last = true;
		h["motion_id"] = id;
	} else {
		h["motion_id"] = uuid;
	}
	h["motion_json"] = json;
	if (first || last)
		h["snapshot_jpeg"] = QString::fromUtf8(snapshot.toBase64());
	push(h);
	noAlarmElapsed.restart();
}

void MotionAlarmSource::notifyNoMotion(const QString &uuid, const QString &json, const QByteArray &snapshot)
{
	lDebug("no motion notification");
	lastAlarmElapsed.restart();
	if (noAlarmElapsed.elapsed() > 90) {
		/* create one last alarm */
		produce(uuid, json, snapshot);
		id = "";
	}
}

void MotionAlarmSource::reset()
{
	id = "";
	AlarmSource::reset();
}

MultipleAlarmSource::MultipleAlarmSource()
{

}

void MultipleAlarmSource::addSource(QSharedPointer<AlarmSource> source)
{
	sources << source;
	source.reset();
}

QList<QSharedPointer<AlarmSource> > MultipleAlarmSource::wait(int msecs)
{
	QList<QSharedPointer<AlarmSource> > active;
	QElapsedTimer t;
	t.start();
	for (int i = 0; i < sources.size(); i++) {
		lDebug("checking source %d for event", i);
		bool event = sources[i]->wait(msecs); //this is not true!
		if (event) {
			lDebug("new event for %d", i);
			active << sources[i];
			break;
		}
		if (t.elapsed() >= msecs)
			break;
	}
	return active;
}

TrackAlarmSource::TrackAlarmSource()
	: AlarmSource()
{

}

QString TrackAlarmSource::typeString() const
{
	return "track_alarm";
}

void TrackAlarmSource::produce(const QString &uuid, const QString &json, const QByteArray &snapshot)
{
	lDebug("producing track event");
	QHash<QString, QVariant> h;
	h["track_id"] = uuid;
	h["track_json"] = json;
	h["snapshot_jpeg"] = QString::fromUtf8(snapshot.toBase64());
	push(h);
}

GenericAlarmSource::GenericAlarmSource(const QString &type)
	: AlarmSource()
{
	this->type = type;
}

QString GenericAlarmSource::typeString() const
{
	return type;
}

void GenericAlarmSource::produce(const QHash<QString, QVariant> &h)
{
	push(h);
}

void GenericAlarmSource::produce(const QString &key, const QVariant &value)
{
	QHash<QString, QVariant> h;
	h.insert(key, value);
	push(h);
}
