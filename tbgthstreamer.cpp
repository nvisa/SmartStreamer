#include "tbgthstreamer.h"

#include "lmm/rtp/rtpreceiver.h"
#include "lmm/rtsp/rtspclient.h"
#include "lmm/tx1/tx1videodecoder.h"
#include "lmm/baselmmpipeline.h"

Tbgthstreamer::Tbgthstreamer(const QJsonObject &config,QObject *parent) : TX1Streamer(parent)
{
	if (!config.isEmpty()) {
		sourceIpAddr = config["source"].toString();
		width = config["resolution_width"].toInt();
		height = config["resolution_height"].toInt();
	}
}

BaseLmmPipeline *Tbgthstreamer::createYUV420Pipeline(QSize &res0)
{
	rtpReceiver = new RtpReceiver(this);
	rtpReceiver->useThreadedReading(true);

	res0.setWidth(width);
	res0.setHeight(height);

	TX1VideoDecoder *dec = new TX1VideoDecoder();

	BaseLmmPipeline *p = addPipeline();
	p->append(rtpReceiver);
	p->append(dec);


	rtspClient = new RtspClient(this);
	rtspClient->setDefaultRtpTrack(rtpReceiver);
	rtspClient->setAuthCredentials("admin","moxamoxa");

	if (sourceIpAddr.startsWith("rtsp://"))
		rtspClient->setServerUrl(QString("%1").arg(sourceIpAddr));
	else
		rtspClient->setServerUrl(QString("rtsp://%1").arg(sourceIpAddr));
	return p;
}
