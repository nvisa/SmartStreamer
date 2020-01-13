#include "tk1streamer.h"
#include "kardelenapi.h"
#include "seiinserter.h"
#include "streamercommon.h"
#include "applicationinfo.h"
#include "alarmgeneratorelement.h"
#include "orioncommunicationserver.h"
#include "algorithm/basealgorithmelement.h"

#include <lmm/debug.h>
#include <lmm/bufferqueue.h>
#include <lmm/rtp/rtpreceiver.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/rtp/rtptransmitter.h>
#include <lmm/pipeline/functionpipeelement.h>

TK1Streamer::TK1Streamer(const QJsonObject &obj, QObject *parent)
	:BaseStreamer(parent)
{
	priv = new TK1StreamerPriv;
	if (!obj.isEmpty()) {
		priv->width = obj.value("width").toInt();
		priv->height = obj.value("height").toInt();
		priv->rtspUser = obj.value("rtsp_user").toString();
		priv->rtspPass = obj.value("rtsp_pass").toString();
		priv->url = obj.value("url").toString();
		priv->fps = obj.value("fps").toDouble();
		priv->orionComm = obj.value("orion_api").toBool();
		priv->receiverType = obj.value("receiver_type").toString();
		priv->decoderType = obj.value("decoder_type").toString();
		priv->rtspTransport = obj.value("rtsp_transport").toString();
		priv->url2 = obj.value("url2").toString();
	}
	rtp2 = NULL;
	rtsp = NULL;
	rtsp2 = NULL;
	algen = NULL;
	queue = new BufferQueue();
	AlgorithmGrpcServer::instance()->setAlgorithmManagementInterface(this);
}

void TK1Streamer::createReceiverEl(QString type)
{
	if (type.isEmpty())
		type = "demux";
	mDebug("selecting receiver type '%s'", qPrintable(type));
	if (type == "demux") {
		rtp = StreamerCommon::createRtspDemux(priv->url, priv->rtspUser, priv->rtspPass, priv->rtspTransport);
		if (!priv->url2.isEmpty())
			rtp2 = StreamerCommon::createRtspDemux(priv->url2, priv->rtspUser, priv->rtspPass, priv->rtspTransport);
	} else if (type == "rtp") {
		rtp = new RtpReceiver(this);
		rtsp = StreamerCommon::createRtspClient(((RtpReceiver*)rtp), priv->url, priv->rtspUser, priv->rtspPass);
		if (!priv->url2.isEmpty()) {
			rtp2 = new RtpReceiver(this);
			rtsp2 = StreamerCommon::createRtspClient(((RtpReceiver*)rtp2), priv->url2, priv->rtspUser, priv->rtspPass);
		}
	}
}

void TK1Streamer::createDecoderEl(QString type)
{
	if (type.isEmpty())
		type = "hw";
	mDebug("selecting decoder element '%s'", qPrintable(type));
	if (type.contains("soft")) {
		decoder = StreamerCommon::createFFmpegDecoder(priv->width, priv->height);
	} else if (type.contains("hw")) {
		decoder = StreamerCommon::createOmxDecoder(priv->fps);
	}
}

int TK1Streamer::start()
{
	createReceiverEl(priv->receiverType);
	createDecoderEl(priv->decoderType);
	BaseLmmPipeline *p1 = generatePipeline();
	if (p1) {
		finishPipeline(p1);

		if (motion->isAutoStart())
			runAlgorithm(0);
		else if (panaroma->isAutoStart())
			runAlgorithm(1);
	}
	return BaseStreamer::start();
}

void TK1Streamer::finishPipeline(BaseLmmPipeline *p1)
{
	sei = new SeiInserter;
	sei->setAlarmTemplate("sei_alarm_template.xml");

	motion = ApplicationInfo::instance()->createAlgorithm("motion");
	panaroma = ApplicationInfo::instance()->createAlgorithm("panaroma");
	if (priv->orionComm) {
		OrionCommunicationServer::instance()->setMotionAlgorithmElement
				((MotionAlgorithmElement *)motion);
		OrionCommunicationServer::instance()->setPanaromaAlgorithmElement
				((PanaromaAlgorithmElement *)panaroma);
	}

	p1->append(motion);
	p1->append(newFunctionPipe(TK1Streamer, this, TK1Streamer::PerformSEI));
	p1->append(panaroma);
	p1->end();

	RtpTransmitter *rtpout = StreamerCommon::createRtpTransmitter(0);
	RtpTransmitter *rtpout2 = StreamerCommon::createRtpTransmitter(0);
	if (priv->receiverType == "rtp") {
		rtpout->forwardRtpTs(true);
		rtpout2->forwardRtpTs(true);
	} else if (priv->receiverType == "demux") {
		rtpout->setUseAbsoluteTimestamp(false);
		rtpout->setFrameRate(priv->fps);
		rtpout2->setUseAbsoluteTimestamp(false);
		rtpout2->setFrameRate(priv->fps);
	}
	StreamerCommon::createRtspServer(rtpout, rtpout2);

	BaseLmmPipeline *p2 = addPipeline();
	p2->append(queue);
	p2->append(sei);
	p2->append(rtpout);
	p2->end();

	if (!priv->url2.isEmpty()) {
		BaseLmmPipeline *psub = addPipeline();
		psub->append(rtp2);
		psub->append(rtpout2);
		psub->end();
	}
	return;
}

int TK1Streamer::PerformSEI(const RawBuffer &buf)
{
	if (sei) {
		if (motion->getState() == BaseAlgorithmElement::PROCESS) {
			QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);
			QByteArray seiData = hash["motion_results"].toByteArray();
			sei->processMessage(seiData);
			if (seiData.size() == 0)
				sei->clearLastSEIMessage();

			if (KardelenAPIServer::instance() && algen) {
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

int TK1Streamer::runAlgorithm(int channel)
{
	BaseAlgorithmElement *el = ApplicationInfo::instance()->getAlgorithmInstance(channel);
	if (el->getState() != BaseAlgorithmElement::UNKNOWN)
		return -EBUSY;
	if (el == motion) {
		if (panaroma->getState() != BaseAlgorithmElement::UNKNOWN)
			return -EINVAL;
		motion->setState(BaseAlgorithmElement::INIT);
	} else if (el == panaroma) {
		if (motion->getState() != BaseAlgorithmElement::UNKNOWN)
			return -EINVAL;
		panaroma->setState(BaseAlgorithmElement::INIT);
	} else {
		// TODO add algos.
	}
	return 0;
}

int TK1Streamer::stopAlgorithm(int channel)
{
	BaseAlgorithmElement *el = ApplicationInfo::instance()->getAlgorithmInstance(channel);
	if (!el)
		return -ENOENT;
	if (el->getState() != BaseAlgorithmElement::PROCESS)
		return -EBUSY;
	el->setState(BaseAlgorithmElement::STOPALGO);
	return 0;
}

TK1Streamer::~TK1Streamer()
{

}

int TK1Streamer::processBuffer(const RawBuffer &buf)
{
	Q_UNUSED(buf);
	return 0;
}
