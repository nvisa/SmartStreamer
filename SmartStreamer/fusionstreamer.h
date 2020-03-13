#ifndef FUSIONSTREAMER_H
#define FUSIONSTREAMER_H

#include "tx1streamer.h"

#include <QJsonArray>
#include <QJsonObject>

#include <opencv/cv.h>

using namespace cv;

struct cropRegions {
	int day_strecth_x_left;
	int day_strecth_y_up;
	int day_strecth_x_right;
	int day_strecth_y_down;
	int thermal_strecth_x_left;
	int thermal_strecth_y_up;
	int thermal_strecth_x_right;
	int thermal_strecth_y_down;
};

class FusionStreamer : public TX1Streamer
{
	Q_OBJECT
public:
	enum StreamMode {
		DAY,
		THERMAL,
		FUSION,
	};

	explicit FusionStreamer(const QJsonObject &config = QJsonObject(), QObject *parent = 0);
	void setMode(StreamMode m);
	void setMode(QString mode);
	QString getMode();

protected:
	virtual BaseLmmPipeline * createYUV420Pipeline(QSize &res0);
	void checkCropRegion(const QJsonObject &config);
	QJsonArray sources;

private:

	int width;
	int height;
	StreamMode mode;
	QStringList sourceList;
	QJsonObject objectJson;
};

#endif // FUSIONSTREAMER_H
