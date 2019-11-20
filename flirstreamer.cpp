#include "flirstreamer.h"

#include "tk1omxpipeline.h"
#include "streamercommon.h"
#include "alarmgeneratorelement.h"

#include <lmm/debug.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/ffmpeg/baselmmdemux.h>
#include <lmm/ffmpeg/ffmpegdecoder.h>


FlirStreamer::FlirStreamer(const QJsonObject &config, QObject *parent)
	:TK1Streamer(config, parent)
{
	algen = new alarmGeneratorElement;
}

BaseLmmPipeline* FlirStreamer::generatePipeline()
{
	dec = new TK1OmxPipeline();
	dec->setPipelineDescription("appsrc name=source ! h264parse ! omxh264dec ! appsink name=sink");
	dec->getSourceCaps(0)->setMime("video/x-h264,stream-format=byte-stream");
	dec->doTimestamp(true, 1000000 / priv->fps);

	BaseLmmPipeline *p1 = addPipeline();
	if (rtpmux)
		p1->append(rtpmux);
	else
		p1->append(rtp);
	p1->append(queue);
	p1->append(dec);
	return p1;
}

int FlirStreamer::pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf)
{
	Q_UNUSED(p);
	Q_UNUSED(buf);
	return 0;
}
