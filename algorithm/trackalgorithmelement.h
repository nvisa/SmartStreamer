#ifndef TRACKALGORITHMELEMENT_H
#define TRACKALGORITHMELEMENT_H

#include "basealgorithmelement.h"


class TrackAlgorithmElement: public BaseAlgorithmElement
{
public:
	TrackAlgorithmElement(QObject *parent = 0);
	int init();
	int processAlgo(const RawBuffer &buf);
	int release();
	enum TrackMode {
		AUTO,
		SEMIAUTO,
		MANUAL
	};

	struct TrackObjInfo {
		float point_x;
		float point_y;
		float width;
		float height;
	};

	struct TrackControl {
		uchar meta[4096];
		float *panTiltZoomRead;
		bool alarmFlag;
		int initialize;
		int sensitivity;
		bool classification;
		TrackObjInfo obj;
	};

	int setTrackObjInfo(float x, float y, float w, float h);
	int ZoomLevelNo;
	float zoomvalues[1000*3];
	void zoom2degree_conversion(int zoomReadOut,float* HV_fovAngles);

protected:
	TrackMode mode;
	TrackControl control;
	int autoTrack(const RawBuffer &buf);
	int semiAutoTrack(const RawBuffer &buf);
	int manualTrack(const RawBuffer &buf);
	int reloadJson(const QJsonObject &node);
	float objProp[4];
};

#endif // TRACKALGORITHMELEMENT_H
