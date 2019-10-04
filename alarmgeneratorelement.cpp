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
	int objectNo = meta[5];
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
		target.topLeftX = meta[baseIndex] + (meta[baseIndex+1] << 8);
		target.topLeftY = meta[baseIndex + 2] + (meta[baseIndex+3] << 8);
		int targetBottomX = meta[baseIndex+4] + (meta[baseIndex+5] << 8);
		int targetBottomY = meta[baseIndex+6] + (meta[baseIndex+7] << 8);
		target.widthOfTarget = abs(target.topLeftX - targetBottomX);
		target.heightOfTarget = abs(target.topLeftY - targetBottomY);
		generatedAlarmInfo->target.push_back(target);
	}
}

void alarmGeneratorElement::generateTrackingStructure(unsigned char meta[])
{
	time_t rawtime;
	struct tm * timeInfo;
	char buffer[80];
	time(&rawtime);
	timeInfo = localtime(&rawtime);
	strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeInfo);
	string str(buffer);
	generatedAlarmInfo->date = str;
	TargetStruct target;

	target.topLeftX = meta[19] + (meta[20] << 8) ;
	target.topLeftY = meta[21] + (meta[22] << 8) ;
	int bottomRightX = meta[23] + (meta[24] << 8) ;
	int bottomRightY = meta[25] + (meta[26] << 8) ;
	target.widthOfTarget = abs(target.topLeftX - bottomRightX);
	target.heightOfTarget = abs(target.topLeftY - bottomRightY);
	generatedAlarmInfo->target.push_back(target);
}
