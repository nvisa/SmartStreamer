#include "flirstreamer.h"

#include "kardelenapi.h"
#include "seiinserter.h"
#include "tk1omxpipeline.h"
#include "streamercommon.h"
#include "applicationinfo.h"
#include "alarmgeneratorelement.h"
#include "algorithm/algorithmgrpcserver.h"
#include "algorithm/motionalgorithmelement.h"

#include <lmm/debug.h>
#include <lmm/bufferqueue.h>
#include <lmm/rtsp/rtspclient.h>
#include <lmm/rtp/rtpreceiver.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/rtsp/basertspserver.h>
#include <lmm/pipeline/functionpipeelement.h>

class FlirStreamerPriv
{
public:
	float fps;
	int width;
	int height;
	QString url;
	QString rtspUser;
	QString rtspPass;
};

FlirStreamer::FlirStreamer(const QJsonObject &config, QObject *parent)
	:BaseStreamer(parent)
{
	priv = new FlirStreamerPriv;
	qDebug() << config;
	if (!config.isEmpty()) {
		priv->width = config.value("width").toInt();
		priv->height = config.value("height").toInt();
		priv->rtspUser = config.value("rtsp_user").toString();
		priv->rtspPass = config.value("rtsp_pass").toString();
		priv->url = config.value("url").toString();
		priv->fps = config.value("fps").toDouble();
	}
	motion = nullptr;
	grpcserv = AlgorithmGrpcServer::instance();
	if (generatePipeline(priv->url))
		mDebug("Defination is missing, please set flir url configuration");

	algen = new alarmGeneratorElement;
}

int FlirStreamer::generatePipeline(const QString &url)
{
	if (url.isEmpty())
		return -ENODATA;

	rtp = new RtpReceiver(this);
	RtspClient *rtsp = StreamerCommon::createRtspClient(rtp, url, priv->rtspUser, priv->rtspPass);

	RtpTransmitter *rtpout = StreamerCommon::createRtpTransmitter(0);
	StreamerCommon::createRtspServer(rtpout);

	BufferQueue *queue = new BufferQueue();

	dec = new TK1OmxPipeline();
	dec->setPipelineDescription("appsrc name=source ! h264parse ! omxh264dec ! appsink name=sink");
	dec->getSourceCaps(0)->setMime("video/x-h264,stream-format=byte-stream");
	dec->doTimestamp(true, 1000000 / priv->fps);

	motion = ApplicationInfo::instance()->createAlgorithm("motion");
	if (motion)
		grpcserv->setMotionAlgorithmElement((MotionAlgorithmElement *)motion);

	sei = new SeiInserter;
	sei->setAlarmTemplate("sei_alarm_template.xml");

	BaseLmmPipeline *p1 = addPipeline();
	p1->append(rtp);
	p1->append(queue);
	p1->append(dec);
	if (motion) {
		p1->append(motion);
		p1->append(newFunctionPipe(FlirStreamer, this, FlirStreamer::PerformSEI));
	}
	p1->end();

	BaseLmmPipeline *p2 = addPipeline();
	p2->append(queue);
	p2->append(rtpout);
	p2->end();

	return 0;
}

int FlirStreamer::PerformSEI(const RawBuffer &buf)
{
	if (sei) {
		if (motion->getState() == BaseAlgorithmElement::PROCESS) {
			QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);
			QByteArray seiData = hash["motion_results"].toByteArray();
			sei->processMessage(seiData);
			if (seiData.size() == 0)
				sei->clearLastSEIMessage();

			if (KardelenAPIServer::instance()) {
				algen->generateAlarmStructure((uchar *)seiData.constData(), alarmGeneratorElement::MOTION);
				alarmGeneratorElement::AlarmInfo *info = algen->getAlarmInfo();
				if (info->target.size())
					KardelenAPIServer::instance()->setMotionObjects(info->target);
			}
		} else
			sei->clearLastSEIMessage();
	}
	return 0;
}

int FlirStreamer::pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf)
{
	Q_UNUSED(p);
	Q_UNUSED(buf);
	return 0;
}
