#ifndef STREAMERCOMMON_H
#define STREAMERCOMMON_H

#include <lmm/rtsp/rtspclient.h>
#include <lmm/rtp/rtptransmitter.h>
#include <lmm/rtsp/basertspserver.h>
#include <lmm/ffmpeg/ffmpegdecoder.h>

class StreamerCommon
{
public:
	StreamerCommon();

	static int detectRtspPort();
	static RtpTransmitter * createRtpTransmitter(float fps);
	static BaseRtspServer * createRtspServer(RtpTransmitter *rtp0);
	static BaseRtspServer * createRtspServer(RtpTransmitter *rtp0, RtpTransmitter *rtp1);
	static BaseRtspServer * createRtspServer(QList<RtpTransmitter*> rtpout);
	static BaseLmmElement * createOverlay();
	static BaseLmmElement * createEncoder(int id = 0);
	static int reloadJson(BaseLmmElement *el);
	static RtspClient *createRtspClient(RtpReceiver *rtp, const QString &url, const QString &user, const QString &pass);
	static FFmpegDecoder *createFFmpegDecoder(int w, int h, int count = 10);
};

#endif // STREAMERCOMMON_H
