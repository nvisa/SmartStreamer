#include "streamercommon.h"

#include <lmm/textoverlay.h>
#include <lmm/tx1/tx1videoencoder.h>

#include <QFile>
#include <QTcpServer>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

static QJsonDocument readJson(const QString &filename)
{
	QFile f(filename);
	if (!f.open(QIODevice::ReadOnly))
		return QJsonDocument();
	QByteArray ba = f.readAll();
	f.close();
	QJsonDocument doc = QJsonDocument::fromJson(ba);
	return doc;
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
	overlay->setProperty("commonTag", "overlay");
	overlay->setPassThru(true);
	reloadJson(overlay);
	return overlay;
}

BaseLmmElement *StreamerCommon::createEncoder(int id)
{
	TX1VideoEncoder *enc = new TX1VideoEncoder;
	enc->setProperty("commonTag", "tx1encoder");
	enc->setProperty("commonTagId", id);
	reloadJson(enc);
	return enc;
}

int StreamerCommon::reloadJson(BaseLmmElement *el)
{
	QString tag = el->property("commonTag").toString();
	int id = el->property("commonTagId").toInt();
	if (tag == "overlay") {
		TextOverlay *overlay = (TextOverlay *)el;
		QJsonObject obj = readJson("/etc/smartstreamer/overlay.json").object();
		if (obj.isEmpty())
			return 0;
		overlay->setPassThru(!obj["enabled"].toBool());
		overlay->setFontSize(obj["fontSize"].toInt());
		overlay->setOverlayPosition(QPoint(obj["x"].toInt(), obj["y"].toInt()));
		overlay->setOverlayText(obj["text"].toString());
		overlay->clearFields();
		QJsonArray arr = obj["fields"].toArray();
		foreach (QJsonValue v, arr) {
			QJsonObject field = v.toObject();
			overlay->addOverlayField(TextOverlay::overlayTextFields(field["id"].toInt()), field["value"].toString());
		}
	} else if (tag == "tx1encoder") {
		TX1VideoEncoder *enc = (TX1VideoEncoder *)el;
		QJsonArray arr = readJson("/etc/smartstreamer/encoders.json").array();
		if (arr.size() <= id)
			return 0;
		QJsonObject obj = arr[id].toObject();
		if (obj.isEmpty())
			return 0;
		int w = 1920, h = 1080;
		if (obj.contains("resolution")) {
			QStringList flds = obj["resolution"].toString().remove("*").split("x");
			w = flds[0].toInt();
			h = 0;
			if (flds.size())
				h = flds[1].toInt();
			if (!w)
				w = 1920;
			if (!h)
				h = 1080;
		}
		enc->setOutputResolution(w, h);
		enc->setBitrate(obj["bitrate"].toInt() * 1000);
		enc->setFps(obj["frameRate"].toInt());
	} else
		return -ENOENT;
	return 0;
}

