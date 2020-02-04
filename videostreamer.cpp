#include "videostreamer.h"

#include "iostream"
#include "inc/BasicStructures.h"

#include <lmm/debug.h>
#include <lmm/videoscaler.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/tools/unittimestat.h>
#include <lmm/tx1/tx1videodecoder.h>
#include <lmm/tx1/tx1videoencoder.h>
#include <lmm/ffmpeg/baselmmdemux.h>
#include <lmm/pipeline/functionpipeelement.h>

#include <QImage>
#include <QPainter>

extern "C" {
#include <libavformat/avformat.h>
}

#include <vector>
#include <memory>
#include <algorithm>

using namespace aselsmart;

static RawBuffer overlayResults(const RawBuffer &buf)
{
	if (buf.constPars()->metaDataRaw == nullptr)
		return buf;

	QImage im;
	if (buf.size() == buf.constPars()->videoWidth * buf.constPars()->videoHeight * 3)
		im = QImage((uchar *)buf.constData(), buf.constPars()->videoWidth, buf.constPars()->videoHeight,
			  QImage::Format_RGB888);
	else
		im = QImage((uchar *)buf.constData(), buf.constPars()->videoWidth, buf.constPars()->videoHeight,
					QImage::Format_RGB32);

	QPainter p(&im);
	p.setPen(Qt::white);
	p.setBrush(Qt::white);
	p.setFont(QFont("Arial", 24));
	p.drawRect(100, 100, 640, 480);
	p.end();

	return buf;
}

static RawBuffer inspect(const RawBuffer &buf)
{
	qDebug() << buf.getMimeType() << buf.size();
	return buf;
}

static RawBuffer libsmart(const RawBuffer &buf)
{
	//static LibSmartContext ctx;
	//ctx.process(buf);
	return buf;
}

class EncoderFeedThread : public QThread
{
public:
	BaseLmmPipeline *p;
	void run()
	{
		QVector<int> randpool;
		for (int i = 0; i < 1024 * 1024; i++) {
			randpool << rand() % 256;
		}
		int rpos = 0;
		sleep(3);
		qDebug("Feeding encoder");
		RawBuffer buf("video/x-raw-yuv", 1920 * 1080 * 3 / 2);
		uchar *data = (uchar *)buf.data();
		QElapsedTimer t;
		t.start();
		while (1) {
			p->getPipe(0)->addBuffer(0, buf);
			int start = rand() % buf.size();
			int end = rand() % (buf.size() - start);
			for (int i = start; i < start + end; i++) {
				data[i] = randpool[rpos++];
				if (rpos == randpool.size())
					rpos = 0;
			}
			qDebug() << "pipeline fps is" << p->getOutputQueue(0)->getFps() << "elapsed:" << t.elapsed();
		}
	}
};

VideoStreamer::VideoStreamer(QObject *parent)
	: BaseStreamer(parent)
{

}
#include <lmm/qtvideooutput.h>
int VideoStreamer::start()
{
#if BENCHMARK
	TX1VideoEncoder *enc = new TX1VideoEncoder;
	BaseLmmPipeline *p1 = addPipeline();
	p1->append(enc);
	//p1->append(new FunctionPipeElement(inspect));
	p1->end();
	EncoderFeedThread *t = new EncoderFeedThread;
	t->p = p1;
	t->start();
	return BaseStreamer::start();
#endif
	//priv->config["srcfile"].toString();
	//QString srcurl = "/etc/smartstreamer_prev0/motbench.ts";
	QString srcurl = "/etc/smartstreamer_prev0/sbt4_5.ts";
	mDebug("Playing video from file %s", qPrintable(srcurl));
	BaseLmmDemux *filesrc = new BaseLmmDemux;
	filesrc->setAudioDemuxing(false);
	filesrc->setLoopFile(true);
	filesrc->setSource(srcurl);
	TX1VideoDecoder *dec = new TX1VideoDecoder;
	dec->setPassThru(false);
	VideoScaler *to420 = new VideoScaler;
	to420->setOutputFormat(AV_PIX_FMT_YUV420P);
	to420->setMode(1);
	VideoScaler* rgbConv1 = new VideoScaler;
	rgbConv1->setOutputFormat(AV_PIX_FMT_ARGB);
	rgbConv1->setMode(1);
	VideoScaler *downsc = new VideoScaler;
	downsc->setOutputResolution(416, 416);
	downsc->setMode(0);

	BaseLmmPipeline *p = addPipeline();
	p->append(filesrc);
	p->append(dec);
	p->append(new FunctionPipeElement(libsmart));
	p->append(rgbConv1);
	p->append(new FunctionPipeElement(overlayResults));
	p->append(new QtVideoOutput);
	p->end();

	/* rate limittting */
	filesrc->getOutputQueue(0)->setRateLimitInterval(0);

	return BaseStreamer::start();
}
