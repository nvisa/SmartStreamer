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

	struct TrackControl {
		uchar meta[4096];
		float *panTiltZoomRead;
		bool alarmFlag;
		int initialize;
		int sensitivity;
		bool classification;
	};

protected:
	BaseAlgorithmCommon::BaseVariables v;
	TrackMode mode;
	TrackControl control;
	int autoTrack(const RawBuffer &buf);
	int semiAutoTrack(const RawBuffer &buf);
	int manualTrack(const RawBuffer &buf);
};

#endif // TRACKALGORITHMELEMENT_H
