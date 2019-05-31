#include "yamgozstreamer.h"
#include "streamercommon.h"
#include "mjpegserver.h"
#include "algorithm/basealgorithmelement.h"

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

extern "C" {
#include <libavformat/avformat.h>
}

static inline void copyyuv(uchar *dst, const uchar *src, int srcpitch, int dstpitch, int height)
{
	for (int i = 0; i < height; i++) {
		memcpy(dst, src, srcpitch);
		src += srcpitch;
		dst += dstpitch;
	}
}

class VideoRect {
public:

	uchar *data;
	int w;
	int h;
	int pitch;

	int size()
	{
		return h * w * 2;
	}

	void copyto(uchar *dst, int dstpitch)
	{
		uchar *src = data;
		for (int i = 0; i < h; i++) {
			memcpy(dst, src, w * 2);
			src += pitch;
			dst += dstpitch;
		}
	}

	VideoRect crop(int x, int y, int cw, int ch)
	{
		VideoRect cr;
		cr.w = cw;
		cr.h = ch;
		cr.pitch = pitch;
		cr.data = data + x * 2 + y * pitch;
		return cr;
	}

	VideoRect crop(QRect r)
	{
		return crop(r.x(), r.y(), r.width(), r.height());
	}
};


class YamgozStreamerPriv
{
public:
	QList<int> stichChannels;
	QList<QRect> stichRects;
};

YamgozStreamer::YamgozStreamer(const QJsonObject &config, QObject *parent)
	: TX1Streamer(parent)
{
	priv = new YamgozStreamerPriv;
	if (config.isEmpty()) {
		priv->stichChannels << 0 << 1 << 2;
		priv->stichRects << QRect();
		priv->stichRects << QRect();
		priv->stichRects << QRect();
	} else {
		priv->stichChannels << config["ch0"].toInt() << config["ch1"].toInt() << config["ch2"].toInt();
		if (config["crop0"].isObject()) {
			QJsonObject obj = config["crop0"].toObject();
			priv->stichRects << QRect(obj["x"].toInt(),
					obj["y"].toInt(),
					obj["width"].toInt(),
					obj["height"].toInt()
					);
		} else
			priv->stichRects << QRect();
		if (config["crop1"].isObject()) {
			QJsonObject obj = config["crop1"].toObject();
			priv->stichRects << QRect(obj["x"].toInt(),
					obj["y"].toInt(),
					obj["width"].toInt(),
					obj["height"].toInt()
					);
		} else
			priv->stichRects << QRect();
		if (config["crop2"].isObject()) {
			QJsonObject obj = config["crop2"].toObject();
			priv->stichRects << QRect(obj["x"].toInt(),
					obj["y"].toInt(),
					obj["width"].toInt(),
					obj["height"].toInt()
					);
		} else
			priv->stichRects << QRect();
	}

#if 0
	if (config["out_mode"].toString() == "vout_only") {
		VideoScaler *toRgb = new VideoScaler;
		toRgb->setOutputFormat(AV_PIX_FMT_ARGB);
		toRgb->setMode(1);

		BaseLmmPipeline *p = addPipeline();
		p->append(multisrc, -1);
		p->append(newFunctionPipe(YamgozStreamer, this, YamgozStreamer::stichFrames));
		p->append(toRgb);
		p->append(new QtVideoOutput);
		p->end();
	}
#endif
}

QSize YamgozStreamer::getStichSize()
{
	VideoRect r0;
	r0.w = 720;
	r0.h = 576;
	if (priv->stichRects[0].isValid())
		r0 = r0.crop(priv->stichRects[0]);
	VideoRect r1;
	r1.w = 720;
	r1.h = 576;
	if (priv->stichRects[1].isValid())
		r1 = r1.crop(priv->stichRects[1]);
	VideoRect r2;
	r2.w = 720;
	r2.h = 576;
	r2.pitch = r2.w * 2;
	if (priv->stichRects[2].isValid())
		r2 = r2.crop(priv->stichRects[2]);
	return QSize(r0.w + r1.w + r2.w, r0.h);
}

int YamgozStreamer::stichFrames(const RawBuffer &buf)
{
	if (buf.getMimeType() != "application/multi-buffer") {
		mDebug("Un-supported buffer mime-type for stiching");
		return 0;
	}
	RawBuffer ch0 = buf.constPars()->subbufs[priv->stichChannels[0]];
	RawBuffer ch1 = buf.constPars()->subbufs[priv->stichChannels[1]];
	RawBuffer ch2 = buf.constPars()->subbufs[priv->stichChannels[2]];

	VideoRect r0;
	r0.data = (uchar *)ch0.data();
	r0.w = ch0.pars()->videoWidth;
	r0.h = ch0.pars()->videoHeight;
	r0.pitch = r0.w * 2;
	if (priv->stichRects[0].isValid())
		r0 = r0.crop(priv->stichRects[0]);

	VideoRect r1;
	r1.data = (uchar *)ch1.data();
	r1.w = ch0.pars()->videoWidth;
	r1.h = ch0.pars()->videoHeight;
	r1.pitch = r1.w * 2;
	if (priv->stichRects[1].isValid())
		r1 = r1.crop(priv->stichRects[1]);

	VideoRect r2;
	r2.data = (uchar *)ch2.data();
	r2.w = ch0.pars()->videoWidth;
	r2.h = ch0.pars()->videoHeight;
	r2.pitch = r2.w * 2;
	if (priv->stichRects[2].isValid())
		r2 = r2.crop(priv->stichRects[2]);

	RawBuffer *mutbuf = (RawBuffer *)&buf;
	int dstW = r0.w + r1.w + r2.w;

	mutbuf->setSize(r0.size() + r1.size() + r2.size());

	uchar *dst = (uchar *)mutbuf->data();
	r0.copyto(dst, dstW * 2);
	//copyyuv(dst, r0.data, r0.pitch, dstW * 2, r0.h);

	dst += r0.w * 2;
	r1.copyto(dst, dstW * 2);
	//copyyuv(dst, r1.data, r1.pitch, dstW * 2, r1.h);

	dst += r1.w * 2;
	r2.copyto(dst, dstW * 2);
	//copyyuv(dst, r2.data, r2.pitch, dstW * 2, r2.h);

	mutbuf->pars()->videoHeight = r0.h;
	mutbuf->pars()->videoWidth = dstW;
	mutbuf->pars()->avPixelFormat = ch0.constPars()->avPixelFormat;
	mutbuf->pars()->v4l2PixelFormat = ch0.constPars()->v4l2PixelFormat;
	mutbuf->pars()->captureTime = ch0.constPars()->captureTime;
	mutbuf->pars()->streamBufferNo = ch0.constPars()->streamBufferNo;

	return 0;
}

BaseLmmPipeline *YamgozStreamer::createYUV420Pipeline(QSize &res0)
{
	MultiBufferSource *multisrc = new MultiBufferSource;
	for (int i = 0; i < 4; i++) {
		V4l2Input *v4l2 = new V4l2Input;
		v4l2->setParameter("videoWidth", 720);
		v4l2->setParameter("videoHeight", 576);
		v4l2->setParameter("device", QString("/dev/video%1").arg(i));
		BaseLmmPipeline *p = addPipeline();
		p->append(v4l2);
		p->append(multisrc, i);
		p->end();
	}

	VideoScaler *to420 = new VideoScaler;
	to420->setOutputFormat(AV_PIX_FMT_YUV420P);
	to420->setMode(1);

	QSize sz = getStichSize();
	res0.setWidth(sz.width());
	res0.setHeight(sz.height());

	BaseLmmPipeline *p = addPipeline();
	p->append(multisrc, -1);
	p->append(newFunctionPipe(YamgozStreamer, this, YamgozStreamer::stichFrames));
	p->append(to420);
	return p;
}

