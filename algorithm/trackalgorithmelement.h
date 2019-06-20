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

	enum TrackStyle {
		DistanceToCenter,
		SizeOfObject,
		UserMode
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
		float trackScore;
		int trackInterval;
		TrackObjInfo obj;
		TrackStyle style;
	};

	int setTrackObjInfo(float x, float y, float w, float h);

	float getTrackScore() { return control.trackScore; }
	void setTrackScore(float s) { control.trackScore = s; }
	int getTrackInterval() { return control.trackInterval; }
	void setTrackInterval(int i) { control.trackInterval = i; }
	int getSensitivity() { return control.sensitivity; }
	void setSensitivity(int sens) { control.sensitivity = sens; }
	void setMode(TrackMode m) { mode = m; }
	int getMode() { return mode; }
	float *getTrackObjInfo() {return objProp;}

	void setTrackStyle(int s) { control.style = (TrackStyle)s; }
	int ZoomLevelNo;
	float zoomvalues[1000*3];
	void zoom2degree_conversion(int zoomReadOut,float* HV_fovAngles);
	int forwardToObjPropFromControl(const TrackControl &control);
protected:
	TrackMode mode;
	TrackControl control;
	int autoTrack(const RawBuffer &buf);
	int semiAutoTrack(const RawBuffer &buf);
	int manualTrack(const RawBuffer &buf);
	int reloadJson(const QJsonObject &node);
	QJsonObject resaveJson(const QJsonObject &node);
	float objProp[4];
};

#endif // TRACKALGORITHMELEMENT_H
