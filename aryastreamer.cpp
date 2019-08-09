#include "aryastreamer.h"

#include <lmm/debug.h>
#include <lmm/videoscaler.h>
#include <lmm/bufferqueue.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/rtp/rtpreceiver.h>
#include <lmm/rtsp/rtspclient.h>
#include <lmm/ffmpeg/ffmpegdecoder.h>
#include <lmm/pipeline/functionpipeelement.h>

#include "seiinserter.h"
#include "streamercommon.h"
#include "snapshotelement.h"
#include "applicationinfo.h"
#include "orioncommunicationserver.h"
#include "algorithm/algorithmgrpcserver.h"
#include "algorithm/motionalgorithmelement.h"
#include "algorithm/panaromaalgorithmelement.h"

class AryaStreamerPriv
{
public:
	float fps;
	int width;
	int height;
	QString url;
	QString rtspUser;
	QString rtspPass;
};

AryaStreamer::AryaStreamer(const QJsonObject &config, QObject *parent)
	: BaseStreamer(parent)
{
	priv = new AryaStreamerPriv;
	if (!config.isEmpty()) {
		priv->width = config.value("width").toInt();
		priv->height = config.value("height").toInt();
		priv->rtspUser = config.value("rtsp_user").toString();
		priv->rtspPass = config.value("rtsp_pass").toString();
		priv->url = config.value("url").toString();
		priv->fps = config.value("fps").toDouble();
	}

	motion = nullptr;
	panaroma = nullptr;
	grpcserv = nullptr;
	grpcorion = nullptr;
	if (config.value("orion_api").toBool())
		grpcorion = OrionCommunicationServer::instance();
	else grpcserv = AlgorithmGrpcServer::instance();

	if (generatePipeline(priv->url))
		 mDebug("Defination is missing, arya configuration must define to `smartconfig.json`");
}

int AryaStreamer::generatePipeline(const QString &url)
{
	if (url.isEmpty())
		return -ENODATA;

	rtp = new RtpReceiver(this);
	RtspClient *rtsp = StreamerCommon::createRtspClient(rtp, url, priv->rtspUser, priv->rtspPass);
	rtsp->setMoxaHacks(true);

	BufferQueue *queue = new BufferQueue();
	BufferQueue *queueDec = new BufferQueue();
	FFmpegDecoder *dec = StreamerCommon::createFFmpegDecoder(priv->width, priv->height);

	sei = new SeiInserter;
	sei->setAlarmTemplate("sei_alarm_template.xml");

	RtpTransmitter *rtpout = StreamerCommon::createRtpTransmitter(0);
	rtpout->forwardRtpTs(true);
	StreamerCommon::createRtspServer(rtpout);

	createAlgorithms();
//	snap = new SnapshotElement();
	if (grpcserv)
		grpcserv->setSnapshotElement(snap);
	else grpcorion->setSnapshotElement(snap);

	VideoScaler *rgbConv2 = new VideoScaler;
	rgbConv2->setMode(1);

	BaseLmmPipeline *p1 = addPipeline();
	p1->append(rtp);
	p1->append(queue);
	p1->append(dec);
	p1->append(queueDec);
	if (motion) {
		p1->append(motion);
		p1->append(newFunctionPipe(AryaStreamer, this, AryaStreamer::PerformSEI));
	}
	if (panaroma)
		p1->append(panaroma);
	p1->end();

	BaseLmmPipeline *p2 = addPipeline();
	p2->append(queue);
	p2->append(sei);
	p2->append(rtpout);
	p2->end();

//	BaseLmmPipeline *p3 = addPipeline();
//	p3->append(queueDec);
//	p3->append(rgbConv2);
//	p3->append(snap);
//	p3->end();
	return 0;
}

int AryaStreamer::PerformSEI(const RawBuffer &buf)
{
	if (sei) {
		if (motion->getState() == BaseAlgorithmElement::PROCESS) {
			QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);
			QByteArray seiData = hash["motion_results"].toByteArray();
			sei->processMessage(seiData);
			if (seiData.size() == 0)
				sei->clearLastSEIMessage();
		} else sei->clearLastSEIMessage();
	}
	return 0;
}

int AryaStreamer::createAlgorithms()
{
	motion = ApplicationInfo::instance()->createAlgorithm("motion");
	if (motion) {
		if (grpcserv)
			grpcserv->setMotionAlgorithmElement((MotionAlgorithmElement *)motion);
		else grpcorion->setMotionAlgorithmElement((MotionAlgorithmElement *)motion);
	}
	panaroma = ApplicationInfo::instance()->createAlgorithm("panaroma");
	if (panaroma) {
		if (grpcserv)
			grpcserv->setPanaromaAlgorithmElement((PanaromaAlgorithmElement *)panaroma);
		else grpcorion->setPanaromaAlgorithmElement((PanaromaAlgorithmElement *)panaroma);
	}
	return 0;
}

int AryaStreamer::pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf)
{
	mInfo("Buffer Size %d", buf.size());
	return 0;
}
