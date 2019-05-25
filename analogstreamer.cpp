#include "analogstreamer.h"
#include "streamercommon.h"
#include "mjpegserver.h"

#include <lmm/debug.h>
#include <lmm/v4l2input.h>
#include <lmm/videoscaler.h>
#include <lmm/bufferqueue.h>
#include <lmm/qtvideooutput.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/multibuffersource.h>
#include <lmm/tx1/tx1jpegencoder.h>
#include <lmm/rtsp/basertspserver.h>
#include <lmm/tx1/tx1videoencoder.h>
#include <lmm/pipeline/functionpipeelement.h>

#include <QJsonArray>

extern "C" {
#include <libavformat/avformat.h>
}

AnalogStreamer::AnalogStreamer(const QJsonObject &config, QObject *parent)
	: BaseStreamer(parent)
{
	QList <RtpTransmitter *> rtpouts;

	QJsonArray arr = config["devices"].toArray();
	if (arr.isEmpty()) {
		QJsonObject src;
		src["device"] = "/dev/video0"; arr << src;
		src["device"] = "/dev/video1"; arr << src;
		src["device"] = "/dev/video2"; arr << src;
		src["device"] = "/dev/video3"; arr << src;
	}

	foreach (QJsonValue v, arr) {
		QJsonObject src = v.toObject();

		V4l2Input *v4l2input = new V4l2Input;
		VideoScaler *yuv = new VideoScaler;
		TX1VideoEncoder *encode = new TX1VideoEncoder;
		BaseLmmPipeline *p = addPipeline();
		RtpTransmitter *rtp = StreamerCommon::createRtpTransmitter(25);
		BufferQueue *jpegq = new BufferQueue;

		v4l2input->setParameter("videoWidth", 720);
		v4l2input->setParameter("videoHeight", 576);
		v4l2input->setParameter("device", src["device"].toString());

		yuv->setOutputFormat(AV_PIX_FMT_YUV420P);
		yuv->setMode(1);

		encode->setBitrate(4000000);
		encode->setFps(25.0);
		encode->setOutputResolution(720, 576);

		p->append(v4l2input);
		p->append(yuv);
		p->append(jpegq);
		p->append(encode);
		p->append(rtp);
		p->end();

		rtpouts << rtp;

		if (src["preview"].toBool()) {
			TX1JpegEncoder *jenc = new TX1JpegEncoder;
			MjpegElement *jpegel = new MjpegElement(13789);
			p = addPipeline();
			p->append(jpegq);
			p->append(jenc);
			p->append(jpegel);
			p->end();
		}
	}

	StreamerCommon::createRtspServer(rtpouts);
}

