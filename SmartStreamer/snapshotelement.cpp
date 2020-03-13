#include "snapshotelement.h"
#include "lmm/debug.h"

#include <QDebug>
#include <QImage>
#include <QBuffer>
#include <QSemaphore>

SnapshotElement::SnapshotElement(QObject *parent)
	: BaseLmmElement(parent)
{
	maxBuffer = 10;
	snapWait.release(maxBuffer);
	mDebug("Snapshot gets default max buffer count %d", snapWait.available());
}

QByteArray SnapshotElement::take(int n)
{
	QByteArray b;
	if (bufList.isEmpty())
		return b;
	b = bufList.takeFirst();
	snapWait.release(n);
	return b;
}

void SnapshotElement::clearList()
{
	bufList.clear();
	snapWait.release(maxBuffer);
}

int SnapshotElement::getValidBufferSize()
{
	return maxBuffer - snapWait.available();
}

QByteArray SnapshotElement::doScreenShot(const RawBuffer &buf)
{
	QByteArray ba;
	QImage im;
	if (buf.size() == buf.constPars()->videoWidth * buf.constPars()->videoHeight * 3)
		im = QImage((const uchar *)buf.constData(), buf.constPars()->videoWidth, buf.constPars()->videoHeight,
					QImage::Format_RGB888);
	else
		im = QImage((const uchar *)buf.constData(), buf.constPars()->videoWidth, buf.constPars()->videoHeight,
					QImage::Format_RGB32);
	QBuffer qbuf(&ba);
	qbuf.open(QIODevice::WriteOnly);
	im.save(&qbuf, "JPG");
	return ba;
}

int SnapshotElement::processBuffer(const RawBuffer &buf)
{
	mInfo("Snapshot element can get buffer count %d", snapWait.available());
	if(snapWait.available()) {
		if (snapWait.tryAcquire())
			bufList << doScreenShot(buf);
	} else {
		clearList();
		bufList << doScreenShot(buf);
	}
	return newOutputBuffer(buf);
}


