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

int UsbStreamer::generatePipelineForOneSource(const QString &sourceUrl)
{
	setupAlgorithmManager();
	V4l2Input* v4l2 = new V4l2Input;
	v4l2->setParameter("videoWidth", 1920);
	v4l2->setParameter("videoHeight", 1080);
	v4l2->setParameter("device", sourceUrl);

	BufferQueue* queue = new BufferQueue;
	BufferQueue* queueSource = new BufferQueue;

	VideoScaler* rgbConv1 = new VideoScaler;
	rgbConv1->setOutputFormat(AV_PIX_FMT_YUV420P);
	rgbConv1->setMode(1);

	AlgorithmElement *faceAlg = new AlgorithmElement(this);
	if(algMan->availableAlgortihms.value(AlgorithmManager::MOTION)) {
		qDebug() << "MOTION";
		//	AlgorithmElement *motionAlg = new AlgorithmElement(this);
		//	motionAlg->setConfigurationElement(algMan->getAlgHandlerFor(0));
		//	motionAlg->setCurrentActiveAlgorithm(AlgorithmElement::MOTION);
		//	algMan->registerAlgorithm(AlgorithmManager::MOTION, motionAlg);
	} else if (algMan->availableAlgortihms.value(AlgorithmManager::TRACKING)) {
		qDebug() << "TRACKING";
		//	AlgorithmElement *trackAlg = new AlgorithmElement(this);
		//	trackAlg->setConfigurationElement(algMan->getAlgHandlerFor(0));
		//	trackAlg->setCurrentActiveAlgorithm(AlgorithmElement::TRACKING);
		//	algMan->registerAlgorithm(AlgorithmManager::TRACKING, trackAlg);
	} else if (algMan->availableAlgortihms.value(AlgorithmManager::STABILIZATION)) {
		qDebug() << "STABILIZATION";
		//	AlgorithmElement *stabilAlg = new AlgorithmElement(this);
		//	stabilAlg->setConfigurationElement(algMan->getAlgHandlerFor(0));
		//	stabilAlg->setCurrentActiveAlgorithm(AlgorithmElement::STABILIZATION);
		//	algMan->registerAlgorithm(AlgorithmManager::STABILIZATION, stabilAlg);
	} else if (algMan->availableAlgortihms.value(AlgorithmManager::FACE_DETECTION)) {
		qDebug() << "FACE_DETECTION";

		faceAlg->setCurrentActiveAlgorithm(AlgorithmElement::FACE_DETECTION);
		algMan->registerAlgorithm(AlgorithmManager::FACE_DETECTION, faceAlg);
		faceAlg->enableAlg(true);
		faceAlg->setConfigurationElement(algMan->getAlgHandlerFor(0));
	}


	BaseLmmPipeline *p1 = addPipeline();
	p1->append(v4l2);
	p1->append(queueSource);
	p1->append(rgbConv1);
	p1->append(queue);


	//VideoScaler *downs = new VideoScaler;
	//downs->setOutputResolution(640, 360);
	//downs->setMode(0);
	//p1->append(downs);

	FFmpegColorSpace *rgbConv3 = new  FFmpegColorSpace;
	rgbConv3->setOutputFormat(AV_PIX_FMT_RGB24);
	p1->append(rgbConv3);
	p1->append(faceAlg);
	p1->append(newFunctionPipe(UsbStreamer, this, UsbStreamer::PerformAlgorithmForYUV));
	p1->end();

	VideoScaler* rgbConv2 = new VideoScaler;
	rgbConv2->setOutputFormat(AV_PIX_FMT_NV12);
	rgbConv2->setMode(1);
	BaseLmmPipeline *p2 = addPipeline();
	p2->append(queueSource);
	p2->append(rgbConv2);
	TX1VideoEncoder *enc = new TX1VideoEncoder;
	enc->setBitrate(4000000);
	enc->setFps(25.0);
	p2->append(enc);

	//sei = new SeiInserter;
	//sei->setAlarmTemplate("sei_alarm_template.xml");

	//p2->append(sei);

	rtpout = new RtpTransmitter(this);
	rtpout->forwardRtpTs(false);
	rtpout->setRtcp(false);
	rtpout->setH264SEIInsertion(true);
	rtpout->useIncomingTimestamp(false);

	p2->append(rtpout);
	p2->end();


	//	VideoScaler *rgbConv3 = new VideoScaler;
	//	rgbConv3->setOutputFormat(AV_PIX_FMT_ARGB);
	//	rgbConv3->setMode(1);

	//	MjpegElement *mjpeg = new MjpegElement(4571);
	//	BaseLmmPipeline *p3 = addPipeline();
	//	VideoScaler *rgbScaler = new VideoScaler;
	//	rgbScaler->setOutputResolution(720, 576);
	//	p3->append(queue);
	//	p3->append(rgbConv3);
	//	//	p3->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::processMainRGB));
	//	p3->append(mjpeg);
	//	p3->end();


	rtspServer = new BaseRtspServer(this, 8554);
	rtspServer->addStream("stream1", false, rtpout);
	rtspServer->addStream("stream1m",true, rtpout, 15678);
	rtspServer->addMedia2Stream("videoTrack", "stream1", false, rtpout);
	rtspServer->addMedia2Stream("videoTrack", "stream1m", true, rtpout);
	qDebug() << "End of Pipeline";
	algMan->startGrpc();
	return 0;
}

int UsbStreamer::setupAlgorithmManager()
{
	algMan = new AlgorithmManager(this);
}

int UsbStreamer::PerformAlgorithmForYUV(const RawBuffer &buf)
{
	if (sei) {
		QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);
		QByteArray sei_data = hash["motion_results"].toByteArray();
		sei->processMessage(sei_data);
		//	if (sei_data.size() == 0)
		//		sei->clearLastSEIMessage();
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

