#ifndef COMMONINFORMATIONINTERFACE
#define COMMONINFORMATIONINTERFACE
enum PTZactions {
	PAN_STOP,
	PAN_RIGHT,
	PAN_LEFT,
	TILT_UP,
	TILT_DOWN,
	TILT_STOP,
	PAN_RIGHT_TILT_UP,
	PAN_RIGHT_TILT_DOWN,
	PAN_LEFT_TILT_UP,
	PAN_LEFT_TILT_DOWN,
	PAN_TILT_POS,
};

struct PTZinformation {
	PTZactions action{PAN_STOP};
	float pan{0};
	float tilt{0};
	uint zoom{0};
};

struct AlarmInfo {
	int baseId;
	int stationId;
	int deviceId;
	int unitType;
	QString date;
	bool alarmFlag;
};

#endif // COMMONINFORMATIONINTERFACE

