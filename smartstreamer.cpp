#include "smartstreamer.h"

#include <lmm/debug.h>
#include <lmm/bufferqueue.h>
#include <lmm/videoscaler.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/rtp/rtpreceiver.h>
#include <lmm/rtsp/rtspclient.h>
#include <lmm/rtp/rtptransmitter.h>
#include <lmm/rtsp/basertspserver.h>
#include <lmm/ffmpeg/ffmpegdecoder.h>
#include <lmm/ffmpeg/ffmpegcolorspace.h>
#include <lmm/pipeline/functionpipeelement.h>

extern "C" {
	#include <libavformat/avformat.h>
}

SmartStreamer::SmartStreamer(QObject *parent)
	: BaseStreamer(parent)
{

}

int SmartStreamer::setupRtspClient(const QString &rtspUrl)
{
	rtp = new RtpReceiver(this);
	rtpout = new RtpTransmitter(this);
	rtpout->setH264SEIInsertion(true);

	/* queues will be used to de-multiplex streams */
	BufferQueue *queue = new BufferQueue;
	BufferQueue *queueScalerEngine = new BufferQueue;

	FFmpegDecoder *dec = new FFmpegDecoder;
	//FFmpegColorSpace *rgbConv = new FFmpegColorSpace;
	//rgbConv->setInputFormat(AV_PIX_FMT_YUV420P);
	//rgbConv->setOutputFormat(AV_PIX_FMT_RGB24);
	VideoScaler *rgbConv2 = new VideoScaler;
	rgbConv2->setMode(1);
	VideoScaler *rgbScaler = new VideoScaler;
	rgbScaler->setOutputResolution(448, 448);
	VideoScaler *yuvScaler = new VideoScaler;
	yuvScaler->setOutputResolution(320, 288);

	BaseLmmPipeline *p1 = addPipeline();
	p1->append(rtp);
	p1->append(queue);
	p1->append(dec);
	//dec->setVideoResolution(720, 578);
	p1->append(queueScalerEngine);
	p1->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::processMainYUV));
	p1->end();

	BaseLmmPipeline *p2 = addPipeline();
	p2->append(queue);
	p2->append(rtpout);
	//p2->append(wss);
	p2->end();

	BaseLmmPipeline *p3 = addPipeline();
	p3->append(queueScalerEngine);
	p3->append(rgbConv2);
	p3->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::processMainRGB));
	p3->append(rgbScaler);
	p3->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::processScaledRGB));
	p3->end();

#if 1
	BaseLmmPipeline *p4 = addPipeline();
	p4->append(queueScalerEngine);
	p4->append(yuvScaler);
	p4->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::processScaledYUV));
	p4->end();
#endif
	rtsp = new RtspClient(this);
	rtsp->addSetupTrack("videoTrack", rtp);
	rtsp->addSetupTrack("trackID=1", rtp);
	rtsp->addSetupTrack("video", rtp);
    //rtsp->setAuthCredentials("admin", "moxamoxa");
	if (rtspUrl.startsWith("rtsp://"))
		rtsp->setServerUrl(QString("%1").arg(rtspUrl));
	else
		rtsp->setServerUrl(QString("rtsp://%1").arg(rtspUrl));

	rtspServer = new BaseRtspServer(this, 8554);
	rtspServer->setNetworkInterface("eth0");
	//rtspServer->setEnabled(true);
	rtspServer->addStream("stream1", false, rtpout);
	rtspServer->addStream("stream1m",true, rtpout, 15678);
	rtspServer->addMedia2Stream("videoTrack", "stream1", false, rtpout);
	rtspServer->addMedia2Stream("videoTrack", "stream1m", true, rtpout);
	//rtsp->setRtspAuthentication((BaseRtspServer::Auth)s->get("video_encoding.rtsp.auth").toInt());

	return 0;
}

int SmartStreamer::processMainYUV(const RawBuffer &buf)
{
	ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
			  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
	return 0;
}

int SmartStreamer::processMainRGB(const RawBuffer &buf)
{
	ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
			  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
	return 0;
}

int SmartStreamer::processScaledRGB(const RawBuffer &buf)
{
	ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
			  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
	return 0;
}

int SmartStreamer::processScaledYUV(const RawBuffer &buf)
{
	ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
			  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
	return 0;
}

int SmartStreamer::pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf)
{
	mInfo("New pipeline %p data: %s - %d bytes", p, qPrintable(buf.getMimeType()), buf.size());

	return 0;
}

