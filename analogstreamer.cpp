#include "analogstreamer.h"
#include "streamercommon.h"

#include <lmm/debug.h>
#include <lmm/v4l2input.h>
#include <lmm/videoscaler.h>
#include <lmm/qtvideooutput.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/multibuffersource.h>
#include <lmm/rtsp/basertspserver.h>
#include <lmm/tx1/tx1videoencoder.h>
#include <lmm/pipeline/functionpipeelement.h>

extern "C" {
#include <libavformat/avformat.h>
}

AnalogStreamer::AnalogStreamer(const QJsonObject &config, QObject *parent)
	: BaseStreamer(parent)
{
	V4l2Input *v4l2 = new V4l2Input;
	v4l2->setParameter("videoWidth", 720);
	v4l2->setParameter("videoHeight", 576);
	if (config.isEmpty())
		v4l2->setParameter("device", QString("/dev/video0"));
	else
		v4l2->setParameter("device", QString(config["device"].toString()));

	VideoScaler *toRgb = new VideoScaler;
	toRgb->setOutputFormat(AV_PIX_FMT_ARGB);
	toRgb->setMode(1);

	VideoScaler *to420 = new VideoScaler;
	to420->setOutputFormat(AV_PIX_FMT_YUV420P);
	to420->setMode(1);

	TX1VideoEncoder *enc = new TX1VideoEncoder;
	enc->setBitrate(4000000);
	enc->setFps(25.0);
	enc->setOutputResolution(720, 576);

	RtpTransmitter *rtpout = StreamerCommon::createRtpTransmitter(25);

	if (config["out_mode"].toString() == "vout_only") {
		BaseLmmPipeline *p = addPipeline();
		p->append(v4l2);
		p->append(toRgb);
		p->append(new QtVideoOutput);
		p->end();
	} else {
		BaseLmmPipeline *p = addPipeline();
		p->append(v4l2);
		p->append(to420);
		p->append(enc);
		p->append(rtpout);
		p->end();
	}

	StreamerCommon::createRtspServer(rtpout);
}

