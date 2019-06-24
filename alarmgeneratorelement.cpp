#include "alarmgeneratorelement.h"

alarmGeneratorElement::alarmGeneratorElement()
{
	generatedAlarmInfo = new AlarmInfo;
	generatedAlarmInfo->baseId = 0;
	generatedAlarmInfo->deviceId = 0;
	generatedAlarmInfo->stationId = 0;
	generatedAlarmInfo->unitType = 0 ;
}

void alarmGeneratorElement::generateAlarmStructure(unsigned char meta[], alarmGeneratorElement::ALARMTYPE type)
{
	if (type == MOTION)
		generateMotionStructure(meta);
	else if(type == TRACKING)
		generateTrackingStructure(meta);
}

alarmGeneratorElement::AlarmInfo *alarmGeneratorElement::getAlarmInfo()
{
	return generatedAlarmInfo;
}

void alarmGeneratorElement::generateMotionStructure(unsigned char meta[])
{
	//int totalByte = meta[0];
	int objectNo = meta[5];
	//int activePolygonNumber = meta[6];
	int sIndex = 8;
	time_t rawtime;
	struct tm * timeInfo;
	char buffer[80];
	time(&rawtime);
	timeInfo = localtime(&rawtime);
	strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeInfo);
	string str(buffer);

	generatedAlarmInfo->target.clear();
	generatedAlarmInfo->date = str;
	for (int i = 0; i < objectNo; ++i) {
		int baseIndex = sIndex + 12 * i;
		TargetStruct target;
		target.topLeftX = meta[baseIndex];
		target.topLeftY = meta[baseIndex + 2];
		target.widthOfTarget = abs(meta[baseIndex] - meta[baseIndex + 4]);
		target.heightOfTarget = abs(meta[baseIndex + 2] - meta[baseIndex + 6]);
		generatedAlarmInfo->target.push_back(target);
	}
}

void alarmGeneratorElement::generateTrackingStructure(unsigned char meta[])
{

}
