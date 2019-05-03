#include "streamercommon.h"

#include <QTcpServer>

StreamerCommon::StreamerCommon()
{

}

int StreamerCommon::detectRtspPort()
{
	QTcpServer server;
	QHostAddress::SpecialAddress bindAddr = QHostAddress::AnyIPv4;
	if (server.listen(bindAddr, 554)) {
		server.close();
		return 554;
	}
	if (server.listen(bindAddr, 8554)) {
		server.close();
		return 8554;
	}
	return 9554;
}

RtpTransmitter *StreamerCommon::createRtpTransmitter(float fps)
{
	RtpTransmitter *rtpout = new RtpTransmitter();
	rtpout->forwardRtpTs(false);
	rtpout->setRtcp(false);
	rtpout->setH264SEIInsertion(true);
	rtpout->useIncomingTimestamp(false);
	if (fps > 0.001) {
		rtpout->setUseAbsoluteTimestamp(false);
		rtpout->setFrameRate(fps);
	} else {
		rtpout->setUseAbsoluteTimestamp(true);
	}
	return rtpout;
}

BaseRtspServer *StreamerCommon::createRtspServer(RtpTransmitter *rtp0)
{
	BaseRtspServer *rtspServer = new BaseRtspServer(NULL, StreamerCommon::detectRtspPort());
	rtspServer->addStream("stream1", false, rtp0);
	rtspServer->addStream("stream1m",true, rtp0, 15678);
	rtspServer->addMedia2Stream("videoTrack", "stream1", false, rtp0);
	rtspServer->addMedia2Stream("videoTrack", "stream1m", true, rtp0);
	return rtspServer;
}

BaseRtspServer *StreamerCommon::createRtspServer(RtpTransmitter *rtp0, RtpTransmitter *rtp1)
{
	BaseRtspServer *rtspServer = new BaseRtspServer(NULL, StreamerCommon::detectRtspPort());
	rtspServer->addStream("stream1", false, rtp0);
	rtspServer->addStream("stream1m",true, rtp0, 15678);
	rtspServer->addMedia2Stream("videoTrack", "stream1", false, rtp0);
	rtspServer->addMedia2Stream("videoTrack", "stream1m", true, rtp0);

	rtspServer->addStream("stream2",false, rtp1);
	rtspServer->addStream("stream2m",true, rtp1, 15680);
	rtspServer->addMedia2Stream("videoTrack", "stream2", false, rtp1);
	rtspServer->addMedia2Stream("videoTrack", "stream2m", true, rtp1);
	return rtspServer;
}

