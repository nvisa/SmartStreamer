#include "flirstreamer.h"
#include "alarmgeneratorelement.h"

#include <lmm/debug.h>
#include <lmm/bufferqueue.h>
#include <lmm/baselmmpipeline.h>

FlirStreamer::FlirStreamer(const QJsonObject &config, QObject *parent)
	:TK1Streamer(config, parent)
{
	algen = new alarmGeneratorElement;
}

BaseLmmPipeline* FlirStreamer::generatePipeline()
{
	BaseLmmPipeline *p1 = addPipeline();
	p1->append(rtp);
	p1->append(queue);
	p1->append(decoder);
	return p1;
}

int FlirStreamer::pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf)
{
	Q_UNUSED(p);
	Q_UNUSED(buf);
	return 0;
}
