#ifndef TBGTHSTREAMER_H
#define TBGTHSTREAMER_H

#include "tx1streamer.h"

#include "QJsonObject"
#include "QSize"

class RtspClient;
class RtpReceiver;
class BaseRtspServer;

class Tbgthstreamer : public TX1Streamer
{
	Q_OBJECT
public:
	explicit Tbgthstreamer(const QJsonObject &config = QJsonObject(),QObject *parent = 0);

protected:
	RtpReceiver *rtpReceiver;
	RtspClient *rtspClient;

	QString sourceIpAddr;
	int width;
	int height;
	virtual BaseLmmPipeline * createYUV420Pipeline(QSize &res0);
};

#endif // TBGTHSTREAMER_H
