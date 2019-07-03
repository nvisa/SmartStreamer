#ifndef VIDEOTESTSOURCESTREAMER_H
#define VIDEOTESTSOURCESTREAMER_H

#include "tx1streamer.h"

#include <QObject>

class VideoTestSourceStreamerPriv;

class VideoTestSourceStreamer : public TX1Streamer
{
	Q_OBJECT
public:
	explicit VideoTestSourceStreamer(const QJsonObject &config = QJsonObject(), QObject *parent = 0);

	virtual int start();

protected slots:
	void frameTimeout();

protected:
	virtual BaseLmmPipeline * createYUV420Pipeline(QSize &res0);
	int generateFrame(const RawBuffer &buf);

	VideoTestSourceStreamerPriv *priv;
};

#endif // VIDEOTESTSOURCESTREAMER_H
