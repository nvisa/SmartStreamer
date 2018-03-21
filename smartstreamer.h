#ifndef SMARTSTREAMER_H
#define SMARTSTREAMER_H

#include <lmm/players/basestreamer.h>

class RtspClient;
class RtpReceiver;
class BaseRtspServer;
class RtpTransmitter;

class SmartStreamer : public BaseStreamer
{
	Q_OBJECT
public:
	explicit SmartStreamer(QObject *parent = 0);

	int setupRtspClient(const QString &rtspUrl);

	int processMainYUV(const RawBuffer &buf);
	int processMainRGB(const RawBuffer &buf);
	int processScaledRGB(const RawBuffer &buf);
	int processScaledYUV(const RawBuffer &buf);
signals:

public slots:
protected:
	int pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf);

	RtpReceiver *rtp;
	RtspClient *rtsp;
	RtpTransmitter *rtpout;
	BaseRtspServer *rtspServer;
};

#endif // SMARTSTREAMER_H
