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

#include "streamercommon.h"

UsbStreamer::UsbStreamer(QObject *parent)
	: BaseStreamer(parent)
{
	grpcserv = AlgorithmGrpcServer::instance();
	startPt("ttyS0?baud=9600;null");
}

int UsbStreamer::generatePipelineForOneSource()
{
	V4l2Input* v4l2 = new V4l2Input;
	v4l2->setParameter("videoWidth", 1920);
	v4l2->setParameter("videoHeight", 1080);
	v4l2->setParameter("device", "/dev/video0");

	BufferQueue* queue = new BufferQueue;

	VideoScaler* rgbConv1 = new VideoScaler;
	rgbConv1->setOutputFormat(AV_PIX_FMT_YUV420P);
	rgbConv1->setMode(1);

	BaseLmmPipeline *p1 = addPipeline();
	p1->setQuitOnThreadError(true);
	p1->append(v4l2);
	p1->append(rgbConv1);
	p1->append(queue);

	motion = new MotionAlgorithmElement();
	motion->setState(BaseAlgorithmElement::UNKNOWN);

	grpcserv->setMotionAlgorithmElement(motion);

	FFmpegColorSpace *rgbConv3 = new  FFmpegColorSpace;
	rgbConv3->setOutputFormat(AV_PIX_FMT_RGB24);
	p1->append(rgbConv3);
	p1->append(motion);
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

	RtpTransmitter *rtpout = StreamerCommon::createRtpTransmitter(25);
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

	RtpTransmitter *rtpout2 = StreamerCommon::createRtpTransmitter(25);
	p3->append(rtpout2);
	p3->end();

	StreamerCommon::createRtspServer(rtpout, rtpout2);
	return 0;
}

int UsbStreamer::PerformAlgorithmForYUV(const RawBuffer &buf)
{
	if (sei) {
		if (motion->getState() == BaseAlgorithmElement::PROCESS) {
			QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);
			QByteArray seiData = hash["motion_results"].toByteArray();
			sei->processMessage(seiData);
			if (seiData.size() == 0)
				sei->clearLastSEIMessage();
		}
	}
	return 0;
}

int UsbStreamer::startPt(const QString &address)
{
	botas = new IRDomeDriver();
	botas->startGrpcApi(50058);
	botas->setTarget(address);
	return 0;
}

int UsbStreamer::processBuffer(const RawBuffer &buf)
{
	Q_UNUSED(buf);
	return 0;
}

int UsbStreamer::pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf)
{
	return 0;
}

