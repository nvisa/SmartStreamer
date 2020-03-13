#include "moxak1streamer.h"

#include "tk1omxpipeline.h"
#include "streamercommon.h"
#include "applicationinfo.h"
#include "algorithm/algorithmgrpcserver.h"
#include "algorithm/motionalgorithmelement.h"
#include "algorithm/stabilizationalgorithmelement.h"
#include "algorithm/trackalgorithmelement.h"

#include "lmm/debug.h"
#include "lmm/bufferqueue.h"
#include "lmm/rtsp/rtspclient.h"
#include "lmm/rtp/rtpreceiver.h"
#include "lmm/baselmmpipeline.h"
#include "lmm/rtsp/basertspserver.h"
#include "lmm/pipeline/functionpipeelement.h"
#include <lmm/x264encoder.h>
#include <lmm/videoscaler.h>
#include <libavformat/avformat.h>
#include <linux/videodev2.h>

MoxaK1Streamer::MoxaK1Streamer(const QJsonObject &config, QObject *parent)
	:BaseStreamer(parent)
{
	sourceInfo = new SourceInformation;
	if (!config.isEmpty()) {
		sourceInfo->sourceIpAddr = config["source"].toString();
		sourceInfo->width = config["resolution_width"].toInt();
		sourceInfo->height = config["resolution_height"].toInt();
		sourceInfo->rtspId = config["rtspId"].toString();
		sourceInfo->rtspPass = config["rtspPass"].toString();
		sourceInfo->fps = config["fps"].toDouble();
	}
	stabilization = nullptr;
	track = nullptr;
	grpcserv = AlgorithmGrpcServer::instance();
	if (generatePipeline(sourceInfo->sourceIpAddr))
		mDebug("Defination is missing, please set tbgth url configuration");

	AlgorithmGrpcServer::instance()->setAlgorithmManagementInterface(this);
}

int MoxaK1Streamer::generatePipeline(const QString &url) {
	if (url.isEmpty())
		return -ENODATA;

	rtp = new RtpReceiver(this);
	RtspClient *rtsp = StreamerCommon::createRtspClient(rtp,sourceInfo->sourceIpAddr,sourceInfo->rtspId,sourceInfo->rtspPass);
	rtsp->setMoxaHacks(true);

	RtpTransmitter *rtpout = StreamerCommon::createRtpTransmitter(0);

	RtpTransmitter *rtpoutEnc = StreamerCommon::createRtpTransmitter(25);

	BufferQueue *queue = new BufferQueue();

	dec = new TK1OmxPipeline();
	dec->setPipelineDescription("appsrc name=source ! h264parse ! omxh264dec ! appsink name=sink");
	dec->getSourceCaps(0)->setMime("video/x-h264,stream-format=byte-stream");
	dec->doTimestamp(true, 1000000 / sourceInfo->fps);


	track = ApplicationInfo::instance()->createAlgorithm("track");
	stabilization = ApplicationInfo::instance()->createAlgorithm("privacy");

	BaseLmmPipeline *p1 = addPipeline();
	p1->append(rtp);
	p1->append(queue);
	p1->append(dec);
	p1->append(track);
	p1->append(stabilization);

	x264Encoder *senc = new x264Encoder;
	senc->setPreset("ultrafast");
	senc->setThreadCount(3);
	senc->setVideoResolution(QSize(sourceInfo->width, sourceInfo->height));
	senc->setBitrate(4000);

	p1->append(senc);
	p1->append(rtpoutEnc);
	p1->end();

	BaseLmmPipeline *p2 = addPipeline();
	p2->append(queue);
	p2->append(rtpout);
	p2->end();

	QList<RtpTransmitter *> rtplist;
	rtplist << rtpoutEnc;
	rtplist << rtpout;

	StreamerCommon::createRtspServer(rtplist);

	return 0;
}

int MoxaK1Streamer::setPixelFormat(const RawBuffer &buf){
	((RawBuffer *)&buf)->pars()->v4l2PixelFormat = V4L2_PIX_FMT_UYVY;
}

int MoxaK1Streamer::runAlgorithm(int channel)
{
	return -EAGAIN;
}

int MoxaK1Streamer::stopAlgorithm(int channel)
{
	return -EAGAIN;
}

int MoxaK1Streamer::pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf) {

	Q_UNUSED(p);
	Q_UNUSED(buf);
	return 0;
}
