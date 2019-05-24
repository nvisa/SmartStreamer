#include "simpleipstreamer.h"
#include "mjpegserver.h"

#include <lmm/debug.h>
#include <lmm/rtp/rtpreceiver.h>
#include <lmm/rtp/rtptransmitter.h>
#include <lmm/rtsp/basertspserver.h>
#include <lmm/rtsp/rtspclient.h>
#include <lmm/tx1/tx1videodecoder.h>
#include <lmm/tx1/tx1jpegencoder.h>
#include <lmm/pipeline/basepipeelement.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/qtvideooutput.h>
#include <lmm/videoscaler.h>
#include <lmm/bufferqueue.h>

extern "C" {
	#include <libavformat/avformat.h>
}

SimpleIPStreamer::SimpleIPStreamer(QObject *parent)
	: BaseStreamer(parent)
{
	RtpReceiver *rtpin = new RtpReceiver;
	//RtpTransmitter *rtpout = new RtpTransmitter;
	TX1VideoDecoder *dec = new TX1VideoDecoder;
	TX1JpegEncoder *jenc = new TX1JpegEncoder;
	VideoScaler *sc = new VideoScaler;
	sc->setMode(1);
	sc->setOutputResolution(1920, 1080);
	sc->setOutputFormat(AV_PIX_FMT_ARGB);
	BufferQueue *queue = new BufferQueue;
	MjpegElement *jpegel = new MjpegElement(13789);

	BaseLmmPipeline *p = addPipeline();
	p->append(rtpin);
	p->append(dec);
	p->append(queue);
	p->append(sc);
	p->append(new QtVideoOutput);
	p->end();

	p = addPipeline();
	p->append(queue);
	p->append(jenc);
	p->append(jpegel);
	p->end();

	RtspClient *rtsp = new RtspClient;
	rtsp->setServerUrl("rtsp://192.168.0.151/Streaming/Channels/101");
	rtsp->setDefaultRtpTrack(rtpin);
}

