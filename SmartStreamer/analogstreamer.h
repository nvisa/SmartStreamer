#ifndef ANALOGSTREAMER_H
#define ANALOGSTREAMER_H

#include "tx1streamer.h"

#include <QJsonArray>
#include <QJsonObject>

class AnalogStreamer : public TX1Streamer
{
	Q_OBJECT
public:
	explicit AnalogStreamer(const QJsonObject &config = QJsonObject(), QObject *parent = 0);

protected:
	virtual BaseLmmPipeline * createYUV420Pipeline(QSize &res0);

	QJsonArray sources;
};

#endif // ANALOGSTREAMER_H
