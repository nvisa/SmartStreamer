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
	setParameters(AlgorithmCommunication::AlarmReqAdvancedParameters());
}

bool AlarmSource::wait(int msecs)
{
	m.lock();
	bool v = wc.wait(&m, msecs);
	m.unlock();
	return v;
}

AlarmSource::QueueData AlarmSource::fetch()
{
	QMutexLocker ml(&m);
	if (queue.size()) {
		auto h = queue.takeFirst();
		fetching(h);
		return h;
	}
	QueueData d;
	return d;
}

bool AlarmSource::check()
{
	QMutexLocker ml(&m);
	if (queue.size())
		return true;
	return false;
}

void AlarmSource::reset()
{
	QMutexLocker ml(&m);
	queue.clear();
}

void AlarmSource::setParameters(const AlgorithmCommunication::AlarmReqAdvancedParameters &pars)
{
	advanced = pars;
	if (!advanced.enabled()) {
		/* set defaults */
		advanced.set_alarmqueuelength(1);
		advanced.set_suppressinterval(90);
		advanced.set_smartsnapshot(true);
		advanced.set_smartsnapshotinterval(1000);
	}
}

void AlarmSource::setParameters(const algorithm::v2::AlarmReqAdvancedParameters &pars)
{
	AlgorithmCommunication::AlarmReqAdvancedParameters pars1;
	pars1.set_alarmqueuelength(pars.alarmqueuelength());
	pars1.set_suppressinterval(pars.suppressinterval());
	pars1.set_smartsnapshot(pars.smartsnapshot());
	pars1.set_smartsnapshotinterval(pars.smartsnapshotinterval());
	pars1.set_enabled(pars.enabled());
	setParameters(pars1);
}

void AlarmSource::fetching(QueueData &)
{
}

void AlarmSource::push(const QueueData &h)
{
	QMutexLocker ml(&m);
	queue << h;
	if (queue.size() > advanced.alarmqueuelength()) {
		queue.removeFirst();
	}
	wc.wakeAll();
	for (int i = 0; i < listeners.size(); i++)
		listeners[i]->notify();
}

void AlarmSource::addSnapshotToAlarm(QueueData &h, const QString &id)
{
	if (alarmCount[id] == 0 ||
			lastSnapshotTime.elapsed() > advanced.smartsnapshotinterval()) {
		/* first time fetching this alarm, put snapshot */
		motex.lock();
		h.hash["snapshot_jpeg"] = QString::fromUtf8(lastSnapshot.toBase64());
		lastSnapshotTime.restart();
		motex.unlock();
	}
	alarmCount[id]++;
}

void AlarmSource::addListener(MultipleAlarmSource *s)
{
	QMutexLocker ml(&m);
	listeners << s;
}

void AlarmSource::removeListener(MultipleAlarmSource *s)
{
	QMutexLocker ml(&m);
	listeners.removeAll(s);
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

void MotionAlarmSource::produce(const QString &uuid, const QString &json, const QByteArray &snapshot, QList<algorithm::v2::DetectedObject> objects)
{
	lDebug("producing motion event");
	QHash<QString, QVariant> h;
	if (uuid.isEmpty()) {
		if (id.isEmpty()) {
			id = QUuid::createUuid().toString();
		}
		h["motion_id"] = id;
	} else {
		h["motion_id"] = uuid;
	}
	h["motion_json"] = json;
	motex.lock();
	lastSnapshot = snapshot;
	noAlarmElapsed.restart();
	motex.unlock();
	QueueData d;
	d.hash = h;
	d.detectedObjects = objects;
	push(d);
}

void MotionAlarmSource::notifyNoMotion(const QString &uuid, const QString &json, const QByteArray &snapshot)
{
	lDebug("no motion notification");
	lastAlarmElapsed.restart();
	if (noAlarmElapsed.elapsed() > advanced.suppressinterval())
		id = "";
}

void MotionAlarmSource::reset()
{
	id = "";
	alarmCount.clear();
	lastSnapshot.clear();
	lastSnapshotTime.restart();
	noAlarmElapsed.restart();
	lastAlarmElapsed.restart();
	AlarmSource::reset();
}

void MotionAlarmSource::fetching(QueueData &h)
{
	addSnapshotToAlarm(h, h.hash["motion_id"].toString());
}

MultipleAlarmSource::MultipleAlarmSource()
{

}

MultipleAlarmSource::~MultipleAlarmSource()
{
	for (int i = 0; i < sources.size(); i++)
		sources[i]->removeListener(this);
}

void MultipleAlarmSource::addSource(QSharedPointer<AlarmSource> source)
{
	sources << source;
	source->addListener(this);
	source.reset();
}

QList<QSharedPointer<AlarmSource> > MultipleAlarmSource::wait(int msecs)
{
	QList<QSharedPointer<AlarmSource> > active;
	mutex.lock();
	if (wc.wait(&mutex, msecs)) {
		/* check active sources */
		for (int i = 0; i < sources.size(); i++) {
			lDebug("checking source %d for event", i);
			if (sources[i]->check())
				active << sources[i];
		}
	}
	mutex.unlock();
	return active;
}

int MultipleAlarmSource::count()
{
	return sources.size();
}

bool MultipleAlarmSource::contains(const QString &type)
{
	for(int i = 0; i < sources.size(); i++)
		if (sources[i]->typeString() == type)
			return true;
	return false;
}

void MultipleAlarmSource::notify()
{
	wc.wakeOne();
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
	motex.lock();
	lastSnapshot = snapshot;
	motex.unlock();
	QueueData d;
	d.hash = h;
	push(d);
}

void TrackAlarmSource::fetching(QueueData &h)
{
	QString id = h.hash["track_id"].toString();
	if (!id.isEmpty())
		addSnapshotToAlarm(h, id);
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

void GenericAlarmSource::produce(const QueueData &h)
{
	push(h);
}

void GenericAlarmSource::produce(const QString &key, const QVariant &value)
{
	QHash<QString, QVariant> h;
	h.insert(key, value);
	QueueData d;
	d.hash = h;
	push(d);
}
