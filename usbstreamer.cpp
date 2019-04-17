#include "usbstreamer.h"
#include "seiinserter.h"
#include "mjpegserver.h"
#include "algorithmmanager.h"
#include "algorithmelement.h"

#include <lmm/debug.h>
#include <lmm/v4l2input.h>
#include <lmm/fileoutput.h>
#include <lmm/bufferqueue.h>
#include <lmm/videoscaler.h>
#include <lmm/qtvideooutput.h>
#include <lmm/tools/unittimestat.h>
#include <lmm/rtp/rtptransmitter.h>
#include <lmm/tx1/tx1videoencoder.h>
#include <lmm/pipeline/functionpipeelement.h>
#include <lmm/rtsp/basertspserver.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/ffmpeg/ffmpegcolorspace.h>

extern "C" {
#include <libavformat/avformat.h>
}

UsbStreamer::UsbStreamer(QObject *parent)
	: BaseStreamer(parent)
{

}

int UsbStreamer::generatePipelineForOneSource(const QString &SourceUrl)
{
	setupAlgorithmManager();
	V4l2Input* v4l2 = new V4l2Input;
	v4l2->setParameter("videoWidth",1920);
	v4l2->setParameter("videoHeight",1080);
	if (QFile::exists("/dev/video0"))
		v4l2->setParameter("device","/dev/video0");
	else if (QFile::exists("/dev/video1"))
		v4l2->setParameter("device","/dev/video1");
	BufferQueue* queue = new BufferQueue;

	VideoScaler* rgbConv1 = new VideoScaler;
	rgbConv1->setOutputFormat(AV_PIX_FMT_YUV420P);
	rgbConv1->setMode(1);

	AlgorithmElement *motionAlg = new AlgorithmElement(this);
	AlgorithmElement *trackAlg  = new AlgorithmElement(this);
	AlgorithmElement *faceAlg   = new AlgorithmElement(this);
	AlgorithmElement *stabilAlg = new AlgorithmElement(this);

	BaseLmmPipeline *p1 = addPipeline();
	p1->setQuitOnThreadError(true);
	p1->append(v4l2);
	p1->append(rgbConv1);
	p1->append(queue);

	FFmpegColorSpace *rgbConv3 = new  FFmpegColorSpace;
	rgbConv3->setOutputFormat(AV_PIX_FMT_RGB24);
	p1->append(rgbConv3);
	if(algMan->availableAlgortihms.value(AlgorithmManager::MOTION)) {
		motionAlg->setConfigurationElement(algMan->getAlgHandlerFor(0),AlgorithmElement::Algorithm::MOTION);
		motionAlg->setCurrentActiveAlgorithm(AlgorithmElement::MOTION);
		algMan->registerAlgorithm(AlgorithmManager::MOTION, motionAlg);
		p1->append(motionAlg);
	}
	if (algMan->availableAlgortihms.value(AlgorithmManager::TRACKING)) {
		trackAlg->setConfigurationElement(algMan->getAlgHandlerFor(0),AlgorithmElement::Algorithm::TRACKING);
		trackAlg->setCurrentActiveAlgorithm(AlgorithmElement::TRACKING);
		algMan->registerAlgorithm(AlgorithmManager::TRACKING, trackAlg);
		p1->append(trackAlg);
	}
	if (algMan->availableAlgortihms.value(AlgorithmManager::STABILIZATION)) {
		stabilAlg->setConfigurationElement(algMan->getAlgHandlerFor(0),AlgorithmElement::Algorithm::STABILIZATION);
		stabilAlg->setCurrentActiveAlgorithm(AlgorithmElement::STABILIZATION);
		algMan->registerAlgorithm(AlgorithmManager::STABILIZATION, stabilAlg);
		p1->append(stabilAlg);
	}
	if (algMan->availableAlgortihms.value(AlgorithmManager::FACE_DETECTION)) {
		faceAlg->setConfigurationElement(algMan->getAlgHandlerFor(0),AlgorithmElement::Algorithm::FACE_DETECTION);
		faceAlg->setCurrentActiveAlgorithm(AlgorithmElement::FACE_DETECTION);
		algMan->registerAlgorithm(AlgorithmManager::FACE_DETECTION, faceAlg);
		p1->append(faceAlg);
	}
	p1->append(newFunctionPipe(UsbStreamer, this, UsbStreamer::PerformAlgorithmForYUV));
	p1->end();

	BaseLmmPipeline *p2 = addPipeline();
	p2->append(queue);
	TX1VideoEncoder *enc = new TX1VideoEncoder;
	enc->setBitrate(4000000);
	enc->setFps(25.0);
	p2->append(enc);

	sei = new SeiInserter;
	sei->setAlarmTemplate("sei_alarm_template.xml");
	p2->append(sei);

	rtpout = new RtpTransmitter(this);
	rtpout->forwardRtpTs(false);
	rtpout->setRtcp(false);
	rtpout->setH264SEIInsertion(true);
	rtpout->useIncomingTimestamp(false);

	p2->append(rtpout);
	p2->end();

	VideoScaler* downScalar = new VideoScaler;
	downScalar->setOutputResolution(640,480);
	downScalar->setOutputFormat(AV_PIX_FMT_NV12);

	BaseLmmPipeline *p3 = addPipeline();
	p3->append(queue);
	p3->append(downScalar);

	TX1VideoEncoder *enc2 = new TX1VideoEncoder;
	enc2->setBitrate(4000000);
	enc2->setFps(25.0);
	enc2->setOutputResolution(640,480);
	p3->append(enc2);

	rtpout2 = new RtpTransmitter(this);
	rtpout2->forwardRtpTs(false);
	rtpout2->setRtcp(false);
	rtpout2->setH264SEIInsertion(false);
	rtpout2->useIncomingTimestamp(false);

	p3->append(rtpout2);
	p3->end();

	rtspServer = new BaseRtspServer(this, 8554);
	rtspServer->addStream("stream1", false, rtpout);
	rtspServer->addStream("stream1m",true, rtpout, 15678);
	rtspServer->addMedia2Stream("videoTrack", "stream1", false, rtpout);
	rtspServer->addMedia2Stream("videoTrack", "stream1m", true, rtpout);
	rtspServer->addStream("stream2",false, rtpout2);
	rtspServer->addStream("stream2m",true, rtpout2, 15680);
	rtspServer->addMedia2Stream("videoTrack", "stream2", false, rtpout2);
	rtspServer->addMedia2Stream("videoTrack", "stream2m", true, rtpout2);
	algMan->startGrpc();

	return 0;
}

int UsbStreamer::setupAlgorithmManager()
{
	algMan = new AlgorithmManager(this);
}

int UsbStreamer::PerformAlgorithmForYUV(const RawBuffer &buf)
{
	if (sei && algMan->availableAlgortihms.value(AlgorithmManager::MOTION)) {
		QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);
		QByteArray sei_data = hash["motion_results"].toByteArray();
		sei->processMessage(sei_data);
		if (sei_data.size() == 0)
			sei->clearLastSEIMessage();
		else {
			AlgorithmCommunication::AlarmInfo alarm;
			alarm.set_baseid(algMan->getAlarmElement().baseId);
			alarm.set_stationid(algMan->getAlarmElement().stationId);
			alarm.set_deviceid(algMan->getAlarmElement().deviceId);
			alarm.set_unittype(algMan->getAlarmElement().unitType);
			if (algMan->getAlgorithmElement(AlgorithmManager::MOTION)->algHandlerEl.meta[0] != 0)
				alarm.set_alarmflag(true);
			else
				alarm.set_alarmflag(false);
			algMan->addAlarm(alarm);
		}
	} else if (algMan->availableAlgortihms.value(AlgorithmManager::TRACKING)) {
		QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);
		QByteArray track_data = hash["track_results"].toByteArray();
		PTZinformation ptzInfo = algMan->getAlgorithmElement(AlgorithmManager::TRACKING)->forwardPTZaction(track_data);
		algMan->setPT(ptzInfo);
		algMan->setZoom(ptzInfo.zoom);
	}
	return 0;
}

int UsbStreamer::processBuffer(const RawBuffer &buf)
{
	return 0;
}

int UsbStreamer::pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf)
{
	return 0;
}

