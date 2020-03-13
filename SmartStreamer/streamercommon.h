#ifndef STREAMERCOMMON_H
#define STREAMERCOMMON_H

#include <lmm/rtsp/rtspclient.h>
#include <lmm/rtp/rtptransmitter.h>
#include <lmm/rtsp/basertspserver.h>
#include <lmm/ffmpeg/ffmpegdecoder.h>
#include <lmm/ffmpeg/baselmmdemux.h>
#include <tk1omxpipeline.h>

class StreamerCommon
{
public:
	StreamerCommon();

	static QJsonDocument readSettingsJSON(const QString &filename);
	static int writeSettingsJSON(const QString &filename, const QJsonDocument &doc);
	static int detectRtspPort();
	static RtpTransmitter * createRtpTransmitter(float fps, int id = 0);
	static BaseRtspServer * createRtspServer(RtpTransmitter *rtp0);
	static BaseRtspServer * createRtspServer(RtpTransmitter *rtp0, RtpTransmitter *rtp1);
	static BaseRtspServer * createRtspServer(QList<RtpTransmitter*> rtpout);
	static BaseLmmElement * createOverlay();
	static BaseLmmElement * createEncoder(int id = 0);
	static int reloadJson(BaseLmmElement *el);
	static RtspClient *createRtspClient(RtpReceiver *rtp, const QString &url, const QString &user, const QString &pass);
	static FFmpegDecoder *createFFmpegDecoder(int w, int h, int count = 10);
	static TK1OmxPipeline * createOmxDecoder(float fps);
	static BaseLmmDemux * createRtspDemux(const QString &url, const QString &user, const QString &pass, const QString &rtspTransport = "");
};

#endif // STREAMERCOMMON_H
