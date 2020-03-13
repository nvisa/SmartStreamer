#ifndef USBSTREAMER_H
#define USBSTREAMER_H

#include "tx1streamer.h"

class UsbStreamer : public TX1Streamer
{
Q_OBJECT
public:
	explicit UsbStreamer(QObject *parent = 0);
protected slots:

protected:
	int pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf);
	BaseLmmPipeline * createYUV420Pipeline(QSize &res0);

};

#endif // USBSTREAMER_H
