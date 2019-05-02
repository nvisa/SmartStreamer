#ifndef TRACKALGORITHMELEMENT_H
#define TRACKALGORITHMELEMENT_H

#include "basealgorithmcommon.h"
#include "basealgorithmelement.h"


class TrackAlgorithmElement: public BaseAlgorithmElement
{
public:
	TrackAlgorithmElement(QObject *parent = 0);
	int init();
	int reallocate();
	int processAlgo(const RawBuffer &buf);
	int baseAlgorithmProcess(const RawBuffer &buf);
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
protected:
	BaseAlgorithmCommon::BaseVariables v;
	TrackMode mode;
	TrackControl control;
	int autoTrack(const RawBuffer &buf);
	int semiAutoTrack(const RawBuffer &buf);
	int manualTrack(const RawBuffer &buf);
	float objProp[4];
};

#endif // TRACKALGORITHMELEMENT_H
