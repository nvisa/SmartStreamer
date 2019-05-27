#include "streamercommon.h"

#include <lmm/textoverlay.h>

#include <QFile>
#include <QTcpServer>
#include <QJsonObject>
#include <QJsonDocument>

static QJsonObject readJson(const QString &filename)
{
	QJsonObject obj;
	QFile f(filename);
	if (!f.open(QIODevice::ReadOnly))
		return obj;
	QByteArray ba = f.readAll();
	f.close();
	QJsonDocument doc = QJsonDocument::fromJson(ba);
	return doc.object();
}

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

BaseRtspServer *StreamerCommon::createRtspServer(QList<RtpTransmitter *> rtpout)
{
	BaseRtspServer *rtspServer = new BaseRtspServer(NULL, StreamerCommon::detectRtspPort());
	for (int i = 0; i < rtpout.size(); i++) {
		rtspServer->addStream(QString("stream%1").arg(i + 1), false, rtpout[i]);
		rtspServer->addStream(QString("stream%1%2").arg(i + 1).arg("m"),true, rtpout[i], 15678 + (2 * i));
		rtspServer->addMedia2Stream("videoTrack", QString("stream%1").arg(i + 1), false, rtpout[i]);
		rtspServer->addMedia2Stream("videoTrack", QString("stream%1%2").arg(i + 1).arg("m"), true, rtpout[i]);
	}
	return rtspServer;
}

BaseLmmElement *StreamerCommon::createOverlay()
{
	TextOverlay *overlay = new TextOverlay(TextOverlay::QPAINTER);
	//QJsonObject obj = readJson("/etc/smartstreamer/overlay.json");
	overlay->setFontSize(48);
	overlay->setOverlayPosition(QPoint(0, 900));
	overlay->setOverlayText("%1 - %2 - %3 - %4");
	overlay->addOverlayField(TextOverlay::FIELD_CURRENT_DATETIME);
	overlay->addOverlayField(TextOverlay::FIELD_STREAM_FPS);
	overlay->addOverlayField(TextOverlay::FIELD_AVG_CPU_LOAD);
	overlay->addOverlayField(TextOverlay::FIELD_STATIC_TEXT, "I made this!");
	return overlay;
}

