#ifndef VIDEOSTREAMER_H
#define VIDEOSTREAMER_H

#include <lmm/players/basestreamer.h>

#include <QObject>

class VideoStreamer : public BaseStreamer
{
	Q_OBJECT
public:
	VideoStreamer(QObject *parent = 0);

	virtual int start();
};

#endif // VIDEOSTREAMER_H
