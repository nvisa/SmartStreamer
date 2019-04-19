#ifndef STREAMERCOMMON_H
#define STREAMERCOMMON_H

#include <lmm/rtp/rtptransmitter.h>
#include <lmm/rtsp/basertspserver.h>

class StreamerCommon
{
public:
	StreamerCommon();

	static int detectRtspPort();
	static RtpTransmitter * createRtpTransmitter(float fps);
	static BaseRtspServer * createRtspServer(RtpTransmitter *rtp0);
};

#endif // STREAMERCOMMON_H
