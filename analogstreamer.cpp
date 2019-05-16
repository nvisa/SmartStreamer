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
	QList <V4l2Input *> v4l2;
	QList <BaseLmmPipeline *> p;
	QList <VideoScaler *> toRgb;
	QList <VideoScaler *> to420;
	QList <QString> videoSource;
	QList <TX1VideoEncoder *> enc;
	QList <RtpTransmitter *> rtpout;

	int numSource = 0;

	if (config.isEmpty()) {
		numSource = 4;
		videoSource << "/dev/video0" << "/dev/video1" << "/dev/video2" << "/dev/video3";
	} else
		numSource = config.size();

	for (int i = 0; i < numSource; i++)
	{
		videoSource << config[QString("device%1").arg(i+1)].toString();
		V4l2Input *v4l2input = new V4l2Input;
		VideoScaler *rgb = new VideoScaler;
		VideoScaler *yuv = new VideoScaler;
		TX1VideoEncoder *encode = new TX1VideoEncoder;
		BaseLmmPipeline *pipeline = addPipeline();
		RtpTransmitter *rtp = StreamerCommon::createRtpTransmitter(25);
		qDebug() << videoSource;

		v4l2 << v4l2input;
		to420 << yuv;	
		toRgb << rgb;
		enc << encode;
		p << pipeline;
		rtpout << rtp;

		v4l2[i]->setParameter("videoWidth", 720);
		v4l2[i]->setParameter("videoHeight", 576);
		v4l2[i]->setParameter("device", videoSource[i]);

		to420[i]->setOutputFormat(AV_PIX_FMT_YUV420P);
		to420[i]->setMode(1);

		enc[i]->setBitrate(4000000);
		enc[i]->setFps(25.0);
		enc[i]->setOutputResolution(720, 576);

		p[i]->append(v4l2[i]);
		p[i]->append(to420[i]);
		p[i]->append(enc[i]);
		p[i]->append(rtpout[i]);
		p[i]->end();
	}
	StreamerCommon::createRtspServer(rtpout);
}

