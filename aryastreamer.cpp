#include "aryastreamer.h"

#include <lmm/debug.h>
#include <lmm/bufferqueue.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/rtsp/rtspclient.h>

AryaStreamer::AryaStreamer(const QJsonObject &config, QObject *parent)
	: TK1Streamer(config, parent)
{
	setObjectName("arya");
}

BaseLmmPipeline * AryaStreamer::generatePipeline()
{
	if (rtsp)
		rtsp->setMoxaHacks(true);

	BaseLmmPipeline *p1 = addPipeline();
	p1->append(rtp);
	p1->append(queue);
	p1->append(decoder);
	return p1;
}

int AryaStreamer::pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf)
{
	Q_UNUSED(p);
	mInfo("Buffer Size %d", buf.size());
	return 0;
}
