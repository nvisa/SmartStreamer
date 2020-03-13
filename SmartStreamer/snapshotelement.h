#ifndef SNAPSHOTELEMENT_H
#define SNAPSHOTELEMENT_H

#include <lmm/baselmmelement.h>
#include "QSemaphore"
#include <QMutex>

class SnapshotElement : public BaseLmmElement
{
	Q_OBJECT
public:
	explicit SnapshotElement(QObject *parent = 0);

	int getValidBufferSize();
	void setMaxBuffer(int v) {maxBuffer = v;}
	QByteArray take(int n = 1);
protected:
	int processBuffer(const RawBuffer &buf);
	QByteArray doScreenShot(const RawBuffer &buf);
	void clearList();
private:
	int maxBuffer;
	QSemaphore snapWait;
	QList<QByteArray> bufList;
};

#endif // SNAPSHOTELEMENT_H
