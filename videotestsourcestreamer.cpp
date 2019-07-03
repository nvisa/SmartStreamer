#include "videotestsourcestreamer.h"

#include <lmm/debug.h>
#include <lmm/v4l2input.h>
#include <lmm/bufferqueue.h>
#include <lmm/videoscaler.h>
#include <lmm/qtvideooutput.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/multibuffersource.h>
#include <lmm/tx1/tx1jpegencoder.h>
#include <lmm/tx1/tx1videoencoder.h>
#include <lmm/pipeline/functionpipeelement.h>

#include <QTimer>
#include <QImage>
#include <QPainter>

extern "C" {
#include <libavformat/avformat.h>
}

class VideoTestSourceStreamerPriv
{
public:
	QImage im;
	QImage imBack;
	QImage overlayIm;
	QJsonObject config;
};

VideoTestSourceStreamer::VideoTestSourceStreamer(const QJsonObject &config, QObject *parent)
	: TX1Streamer(parent)
{
	priv = new VideoTestSourceStreamerPriv;
	priv->im = QImage(1920, 1080, QImage::Format_ARGB32);
	priv->config = config;
	if (config["background_image"].toString() != "")
		priv->imBack = QImage(config["background_image"].toString()).convertToFormat(QImage::Format_ARGB32).scaledToWidth(1920);
	if (config["overlay_image"].toString() != "")
		priv->overlayIm = QImage(config["overlay_image"].toString()).convertToFormat(QImage::Format_ARGB32).scaledToWidth(256);
}

int VideoTestSourceStreamer::start()
{
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), SLOT(frameTimeout()));
	timer->start(40);
	return TX1Streamer::start();
}

void VideoTestSourceStreamer::frameTimeout()
{
	static int count = 0;
	RawBuffer buf("video/x-raw-rgb", 1920 * 1080 * 4);
	buf.pars()->avPixelFormat = AV_PIX_FMT_ARGB;
	buf.pars()->videoWidth = 1920;
	buf.pars()->videoHeight = 1080;
	buf.pars()->streamBufferNo = count++;

	int x = (count * 20) & ((1 << 10) - 1);
	int y = (count * 10) & ((1 << 9) - 1);
	QPainter p(&priv->im);
	p.setPen(Qt::black);
	p.setBrush(Qt::black);
	if (priv->imBack.isNull())
		p.fillRect(priv->im.rect(), Qt::yellow);
	else
		p.drawImage(0, 0, priv->imBack);
	if (priv->overlayIm.isNull())
		p.drawRect(x, y, 256, 256);
	else
		p.drawImage(x, y, priv->overlayIm);
	memcpy(buf.data(), priv->im.bits(), buf.size());

	getPipeline(0)->getPipe(0)->addBuffer(0, buf);
}

BaseLmmPipeline *VideoTestSourceStreamer::createYUV420Pipeline(QSize &res0)
{
	res0.setWidth(1920);
	res0.setHeight(1080);

	VideoScaler *to420 = new VideoScaler;
	to420->setOutputFormat(AV_PIX_FMT_YUV420P);
	to420->setMode(1);

	BaseLmmPipeline *p = addPipeline();
	//p->append(newFunctionPipe(VideoTestSourceStreamer, this, VideoTestSourceStreamer::generateFrame));
	p->append(to420);
	return p;
}

int VideoTestSourceStreamer::generateFrame(const RawBuffer &buf)
{
	return 0;
}

