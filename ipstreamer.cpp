#include "ipstreamer.h"
#include "mjpegserver.h"
#include "seiinserter.h"
#include "algorithmmanager.h"
#include "algorithmelement.h"
#include "streamercommon.h"

#include <ecl/debug.h>

#include <lmm/debug.h>
#include <lmm/bufferqueue.h>
#include <lmm/videoscaler.h>
#include <lmm/qtvideooutput.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/rtp/rtpreceiver.h>
#include <lmm/rtsp/rtspclient.h>
#include <lmm/tools/unittimestat.h>
#include <lmm/rtp/rtptransmitter.h>
#include <lmm/rtsp/basertspserver.h>
#include <lmm/ffmpeg/ffmpegdecoder.h>
#include <lmm/ffmpeg/ffmpegcolorspace.h>
#include <lmm/pipeline/functionpipeelement.h>

#include <lmm/tx1/tx1videodecoder.h>
#include <lmm/tx1/tx1videoencoder.h>
#include <lmm/v4l2output.h>
#include <lmm/v4l2input.h>
#include <lmm/x264encoder.h>
#include <lmm/ffmpeg/baselmmdemux.h>

extern "C" {
#include <libavformat/avformat.h>
}


IpStreamer::IpStreamer(const QJsonObject &config, QObject *parent):
	TX1Streamer(parent)
{
	this->config = config;
}

void IpStreamer::setCurrentSource(const QString &url)
{
	mDebug("Switching video source to %s", qPrintable(url));
	BaseLmmDemux *demux = (BaseLmmDemux *)getPipeline(0)->getPipe(0);
	demux->restart(url);
}

QString IpStreamer::getCurrentSource()
{
	return currentSourceUrl;
}

int IpStreamer::pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf)
{
	//qDebug() << buf.size() << buf.getMimeType();
	return 0;
}

BaseLmmPipeline *IpStreamer::createYUV420Pipeline(QSize &res0)
{
	QString rtspUrl = config["rtspUrl"].toString();
	if (rtspUrl.isEmpty())
		rtspUrl = "rtsp://admin:admin@192.168.1.108/stream1";
	QString rtspTransport = config["rtspTransport"].toString();
	BaseLmmDemux *rtp = new BaseLmmDemux;
	if (!rtspTransport.isEmpty())
		rtp->setParameter("rtsp_transport", rtspTransport);
	rtp->setSource(rtspUrl);
	currentSourceUrl = rtspUrl;

	int width = config["outputWidth"].toInt();
	if (!width)
		width = 1920;
	int height = config["outputHeight"].toInt();
	if (!height)
		height = 1080;
	res0.setWidth(width);
	res0.setHeight(height);

	TX1VideoDecoder *dec = new TX1VideoDecoder();

	VideoScaler *sc0 = new VideoScaler;
	sc0->setMode(0);
	sc0->setOutputResolution(width, height);

	rtpout = StreamerCommon::createRtpTransmitter(0, 5);

	BaseLmmPipeline *p = addPipeline();
	p->append(rtp);
	p->append(rtpout);
	p->append(dec);
	p->append(sc0);

	return p;
}

void IpStreamer::addExtraRtpTransmitters(QList<RtpTransmitter *> &list)
{
	list << rtpout;
}

