#ifndef ANALOGSTREAMER_H
#define ANALOGSTREAMER_H

#include <lmm/players/basestreamer.h>

#include <QJsonObject>

class AnalogStreamer : public BaseStreamer
{
	Q_OBJECT
public:
	explicit AnalogStreamer(const QJsonObject &config = QJsonObject(), QObject *parent = 0);

signals:

public slots:
};

#endif // ANALOGSTREAMER_H
