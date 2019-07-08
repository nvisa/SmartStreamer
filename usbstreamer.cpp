#include "usbstreamer.h"

#include <lmm/debug.h>
#include <lmm/v4l2input.h>
#include <lmm/videoscaler.h>
#include <lmm/baselmmpipeline.h>

#include <QSize>

extern "C" {
#include <libavformat/avformat.h>
}

#include "streamercommon.h"

UsbStreamer::UsbStreamer(QObject *parent)
	: TX1Streamer(parent)
{

}

int UsbStreamer::pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf)
{
	Q_UNUSED(p);
	Q_UNUSED(buf);
	return 0;
}

BaseLmmPipeline *UsbStreamer::createYUV420Pipeline(QSize &res0)
{
	V4l2Input* v4l2 = new V4l2Input;
	v4l2->setParameter("videoWidth", 1920);
	v4l2->setParameter("videoHeight", 1080);
	v4l2->setParameter("device", "/dev/video0");
	res0.setWidth(1920);
	res0.setHeight(1080);
	v4l2->dropMalformed(true);

	VideoScaler* rgbConv1 = new VideoScaler;
	rgbConv1->setOutputFormat(AV_PIX_FMT_YUV420P);
	rgbConv1->setMode(1);

	BaseLmmPipeline *p1 = addPipeline();
	p1->append(v4l2);
	p1->append(rgbConv1);
	return p1;
}

