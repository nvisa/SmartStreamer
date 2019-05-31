#ifndef YAMGOZSTREAMER_H
#define YAMGOZSTREAMER_H

#include "tx1streamer.h"

#include <QJsonObject>

class YamgozStreamerPriv;

class YamgozStreamer : public TX1Streamer
{
	Q_OBJECT
public:
	explicit YamgozStreamer(const QJsonObject &config = QJsonObject(), QObject *parent = 0);

protected:
	QSize getStichSize();
	int stichFrames(const RawBuffer &buf);
	virtual BaseLmmPipeline * createYUV420Pipeline(QSize &res0);

	YamgozStreamerPriv *priv;
};

#endif // YAMGOZSTREAMER_H
