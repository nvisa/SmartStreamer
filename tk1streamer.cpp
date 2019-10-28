#include "tk1streamer.h"
#include "kardelenapi.h"
#include "seiinserter.h"
#include "streamercommon.h"
#include "applicationinfo.h"
#include "alarmgeneratorelement.h"
#include "algorithm/basealgorithmelement.h"

#include <lmm/debug.h>
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
	}

	algen = NULL;
	AlgorithmGrpcServer::instance()->setAlgorithmManagementInterface(this);
}

int TK1Streamer::start()
{
	BaseLmmPipeline *p1 = generatePipeline();
	if (p1) {
		finishPipeline(p1);
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
	if (objectName() != "arya") {
		rtpout->forwardRtpTs(true);
	}
	StreamerCommon::createRtspServer(rtpout);

	BaseLmmPipeline *p2 = addPipeline();
	p2->append(queue);
	p2->append(sei);
	p2->append(rtpout);
	p2->end();
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
