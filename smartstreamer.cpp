#include "smartstreamer.h"
#include "mjpegserver.h"

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

#ifdef HAVE_VIA_WRAPPER
#include <viawrapper.h>
#endif

extern "C" {
	#include <libavformat/avformat.h>
}

#define PRINT_BUFS 0

SmartStreamer::SmartStreamer(QObject *parent)
	: BaseStreamer(parent)
{
#ifdef HAVE_VIA_WRAPPER
	wrap = new ViaWrapper();
#endif
}

int SmartStreamer::setupRtspClient(const QString &rtspUrl)
{
	rtp = new RtpReceiver(this);
	rtp->useThreadedReading(true);
	rtpout = new RtpTransmitter(this);
	rtpout->setH264SEIInsertion(true);
	rtpout->useIncomingTimestamp(false);

	/* queues will be used to de-multiplex streams */
	BufferQueue *queue = new BufferQueue;
	BufferQueue *queueScalerEngine = new BufferQueue;

	vout = new QtVideoOutput;
	dec = new FFmpegDecoder;
	dec->setBufferCount(60);
	//dec->setVideoResolution(720, 578);
	VideoScaler *rgbConv2 = new VideoScaler;
	rgbConv2->setMode(1);
	VideoScaler *rgbScaler = new VideoScaler;
	rgbScaler->setOutputResolution(640, 360);
	VideoScaler *yuvScaler = new VideoScaler;
	yuvScaler->setOutputResolution(1280, 720);

	BaseLmmPipeline *p1 = addPipeline();
	p1->append(rtp);
	p1->append(queue);
	p1->append(dec);
	p1->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::checkPoint));
	p1->append(queueScalerEngine);
	p1->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::processMainYUV));
	p1->end();
	rtp->getOutputQueue(0)->setBufferDuration(33);
	rtp->getOutputQueue(0)->setTimestampingMethod(ElementIOQueue::TS_DURATION);

	BaseLmmPipeline *p2 = addPipeline();
	p2->append(queue);
	p2->append(rtpout);
	//p2->append(wss);
	p2->end();

	MjpegElement *mjpeg = new MjpegElement(4571);
	BaseLmmPipeline *p3 = addPipeline();
	p3->append(queueScalerEngine);
	p3->append(rgbConv2);
	p3->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::processMainRGB));
	p3->append(vout);
	p3->append(rgbScaler);
	p3->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::processScaledRGB));
	p3->append(mjpeg);
	p3->end();

#if 0
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
	rtspServer->addStream("stream1", false, rtpout);
	rtspServer->addStream("stream1m",true, rtpout, 15678);
	rtspServer->addMedia2Stream("videoTrack", "stream1", false, rtpout);
	rtspServer->addMedia2Stream("videoTrack", "stream1m", true, rtpout);
	//rtsp->setRtspAuthentication((BaseRtspServer::Auth)s->get("video_encoding.rtsp.auth").toInt());

	return 0;
}

int SmartStreamer::processMainYUV(const RawBuffer &buf)
{
	if (PRINT_BUFS)
		ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
			  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
#ifdef HAVE_VIA_WRAPPER
	if (buf.constPars()->streamBufferNo % 6 == 0) {
		QElapsedTimer t; t.start();
		wrap->viaEgo(buf);
		ffDebug() << t.elapsed() << buf.constPars()->streamBufferNo;
	}
#endif
	return 0;
}

int SmartStreamer::processMainRGB(const RawBuffer &buf)
{
	if (PRINT_BUFS)
		ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
			  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
	return 0;
}

int SmartStreamer::processScaledRGB(const RawBuffer &buf)
{
	if (PRINT_BUFS)
		ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
			  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
	return 0;
}

int SmartStreamer::processScaledYUV(const RawBuffer &buf)
{
	if (PRINT_BUFS)
		ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
			  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
	return 0;
}

int SmartStreamer::checkPoint(const RawBuffer &buf)
{
	Q_UNUSED(buf);
	static UnitTimeStat stat;
	stat.addStat();
	ffDebug() << stat.min << stat.max << stat.avg;
	return 0;
}

void SmartStreamer::timeout()
{
	qDebug() << rtp->getOutputQueue(0)->getFps()
				<< dec->getOutputQueue(0)->getFps()
				<< vout->getWidget()->getDropCount();
	PipelineManager::timeout();
}

int SmartStreamer::pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf)
{
	mInfo("New pipeline %p data: %s - %d bytes", p, qPrintable(buf.getMimeType()), buf.size());
	if (PRINT_BUFS && p == getPipeline(0))
		qDebug() << buf.size() << buf.getMimeType();

	return 0;
}

