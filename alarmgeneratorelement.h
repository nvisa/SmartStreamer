#ifndef ALARMGENERATORELEMENT_H
#define ALARMGENERATORELEMENT_H

#include <string>
#include <uchar.h>
#include <vector>
#include <math.h>
#include <ctime>
using namespace std;
class alarmGeneratorElement
{
public:

	enum ALARMTYPE {
		MOTION,
		TRACKING
	};

	struct TargetStruct {
		enum TargetClass {
			HUMAN,
			CAR,
			OTHER
		};
		int topLeftX;
		int topLeftY;
		int widthOfTarget;
		int heightOfTarget;
		TargetClass targetClass;

	};
	struct AlarmInfo {
		int baseId;
		int stationId;
		int deviceId;
		int unitType;
		string date;
		bool alarmFlag;
		vector<TargetStruct> target;
		//byte frameOfAlarm8;
	};
	alarmGeneratorElement();
	void generateAlarmStructure(unsigned char meta[],ALARMTYPE type);

	AlarmInfo * getAlarmInfo();

private:
	void generateMotionStructure(unsigned char meta[]);
	void generateTrackingStructure(unsigned char meta[]);
	AlarmInfo *generatedAlarmInfo;
};

#endif // ALARMGENERATORELEMENT_H
