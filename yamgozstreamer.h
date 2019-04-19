#ifndef YAMGOZSTREAMER_H
#define YAMGOZSTREAMER_H

#include <lmm/players/basestreamer.h>

#include <QJsonObject>

class YamgozStreamerPriv;

class YamgozStreamer : public BaseStreamer
{
	Q_OBJECT
public:
	explicit YamgozStreamer(const QJsonObject &config = QJsonObject(), QObject *parent = 0);

protected:
	int stichFrames(const RawBuffer &buf);

	YamgozStreamerPriv *priv;
};

#endif // YAMGOZSTREAMER_H
