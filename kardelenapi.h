#ifndef KARDELENAPI_H
#define KARDELENAPI_H

#include "alarmgeneratorelement.h"
#include "proto/KardelenAPI.grpc.pb.h"
#include "algorithm/algorithmgrpcserver.h"

#include <QMutex>
#include <QString>

class PtzpDriver;
class KardelenAPIImpl;

namespace kaapi {

const static int CAPABILITY_JOYSTICK_CONTROL = 0;
const static int CAPABILITY_DETECTION = 1;
const static int CAPABILITY_TRACKING = 2;
const static int CAPABILITY_SCANNING = 3;
const static int CAPABILITY_MISSION_EXECUTION = 4;//
const static int CAPABILITY_ZOOM = 5;
const static int CAPABILITY_FOCUS = 6;
const static int CAPABILITY_BRIGHTNESS = 7;
const static int CAPABILITY_CONTRAST = 8;
const static int CAPABILITY_HUE = 9;
const static int CAPABILITY_SATURATION = 10;
const static int CAPABILITY_SHARPNESS = 11;
const static int CAPABILITY_POLARITY = 12;
const static int CAPABILITY_PT = 13;//
const static int CAPABILITY_MAST = 14;//
const static int CAPABILITY_ROI = 15;
const static int CAPABILITY_DAY_VIEW = 16;
const static int CAPABILITY_NIGHT_VIEW = 17;
const static int CAPABILITY_RANGE = 18;

/* IYAKIN:CAMERA */
const static int CAPABILITY_SENSITIVITY_ADJUSTMENT = 19;
/* IYAKIN:CAMERA */

const static int CAPABILITY_BORESEIGHT = 20;
const static int CAPABILITY_DIGITAL_ZOOM = 21;
const static int CAPABILITY_MENU_OVER_VIDEO = 22;
const static int CAPABILITY_LAZER_RANGE_FINDER = 23;
const static int CAPABILITY_SHOW_HIDE_SEMBOLOGY = 24;
const static int CAPABILITY_CALIBRATION = 25;
const static int CAPABILITY_SHUT_DOWN = 26;
const static int CAPABILITY_RESTART = 27;
const static int CAPABILITY_AUTO_TRACK_WINDOW = 28;
const static int CAPABILITY_VIRTUAL_FENCE = 29;
const static int CAPABILITY_AUTO_TRACK_DETECTION = 30;
const static int CAPABILITY_NUC = 31;
const static int CAPABILITY_LIMIT_YAW_PITCH = 32; //
const static int CAPABILITY_HARD_CALIBRATION = 33; //
const static int CAPABILITY_HARD_LIMIT = 34; //
const static int CAPABILITY_ROLL = 35; //

/*kgys mobil pts için eklendi*/
const static int CAPABILITY_SCENARIO = 36;
// SARP ATIŞ MESAJI İÇİN EKLENDİ
const static int CAPABILITY_FIRE = 37;
const static int CAPABILITY_HPF_GAIN = 38;
const static int CAPABILITY_HPF_SPATIAL = 39;

const static int NUM_PARAM_SENSITIVITY = 0;
const static int NUM_PARAM_RANGE = 1;
const static int NUM_PARAM_HEIGHT = 2;
const static int NUM_PARAM_FOV = 3;
const static int NUM_PARAM_FOCUS = 4;
const static int NUM_PARAM_BRIGHTNESS = 5;
const static int NUM_PARAM_CONTRAST = 6;
const static int NUM_PARAM_HUE = 7;
const static int NUM_PARAM_SATURATION = 8;
const static int NUM_PARAM_SHARPNESS = 9;
const static int NUM_PARAM_YAW = 10; //
const static int NUM_PARAM_PITCH = 11; //
const static int NUM_PARAM_HORIZONTAL_RES = 12;
const static int NUM_PARAM_VERTICAL_RES = 13;
const static int NUM_PARAM_ZOOM = 14;
const static int NUM_PARAM_LASER_RANGE = 15;
const static int NUM_PARAM_TRACK_WINDOW_WIDTH = 16;
const static int NUM_PARAM_TRACK_WINDOW_HEIGHT = 17;
const static int NUM_PARAM_DIGITAL_ZOOM_WINDOW_WIDTH = 18;
const static int NUM_PARAM_DIGITAL_ZOOM_WINDOW_HEIGHT = 19;
const static int NUM_PARAM_YAW_LIMIT_START = 20; //
const static int NUM_PARAM_YAW_LIMIT_FINISH = 21; //
const static int NUM_PARAM_PITCH_LIMIT_START = 22; //
const static int NUM_PARAM_PITCH_LIMIT_FINISH = 23; //
const static int NUM_PARAM_ROLL = 24; //
const static int NUM_PARAM_PREDEFINED_FOV_COUNT = 25; //
const static int NUM_PARAM_BULLET_COUNT = 26;
const static int NUM_PARAM_HPF_GAIN = 27;
const static int NUM_PARAM_HPF_SPATIAL = 28;
const static int NUM_PARAM_PREDEFINED_GAIN_COUNT = 29; //

const static int VALUE_NO_CHANGE = 0;
const static int VALUE_INCREASE = 1;
const static int VALUE_DECREASE = 2;
const static int VALUE_SET = 3;

const static int ENUM_PARAM_SENSITIVITY_ADJUSTMENT_TYPE = 0;
typedef int byte;
const static byte AUTO_MODE = (byte) 0x01;
const static byte MANUAL_MODE = (byte) 0x02;
/* IYAKIN:CAMERA */

const static int ENUM_PARAM_CAMERA_TYPE = 1;
const static byte TV = (byte) 0x01;
const static byte THERMAL = (byte) 0x02;

const static int ENUM_PARAM_OPERATIONAL_MODE = 2;
const static byte CONTROL_MODE_WATCH = (byte) 0x00;
const static byte CONTROL_MODE_JOYSTICK = (byte) 0x01;
const static byte CONTROL_MODE_DETECTION = (byte) 0x02;
const static byte CONTROL_MODE_TRACK_WINDOW_SELECT = (byte) 0x03;
const static byte CONTROL_MODE_TRACK_STARTED = (byte) 0x04;
const static byte CONTROL_MODE_SCANNING = (byte) 0x05;
const static byte CONTROL_MODE_MISSION = (byte) 0x06;
const static byte CONTROL_MODE_DIGITAL_ZOOM_WINDOW_SELECT = (byte) 0x07;
const static byte CONTROL_MODE_DIGITAL_ZOOM_STARTED = (byte) 0x08;
const static byte CONTROL_MODE_VIRTUAL_FENCE = (byte) 0x09;
const static byte CONTROL_MODE_PAN_CHANGE_STARTED = (byte) 0x0a;
/* IYAKIN:CAMERA */
const static int ENUM_PARAM_DETECTION_CREATION_MODE = 3;
const static byte DETECTION_OPEN_MODE = (byte) 0x01;
const static byte DETECTION_CLOSED_MODE = (byte) 0x02;
/* IYAKIN:CAMERA */

const static int ENUM_PARAM_POLARITY = 4;
const static byte WHITE_HOT = (byte) 0x00;
const static byte BLACK_HOT = (byte) 0x01;

const static int ENUM_PARAM_SEMBOLOGY = 5;
const static byte SYMBOLOGY_ON = (byte) 0x01;
const static byte SYMBOLOGY_OFF = (byte) 0x02;

const static int ENUM_PARAM_TRACK_WINDOW_TYPE = 6;
const static byte TRACK_WINDOW_DYNAMIC_SIZE = (byte) 0x01;
const static byte TRACK_WINDOW_FIXED_SIZE = (byte) 0x03;

const static int ENUM_PARAM_DIGITAL_ZOOM_WINDOW_TYPE = 7;
const static byte DIGITAL_ZOOM_WINDOW_DYNAMIC_SIZE = (byte) 0x01;
const static byte DIGITAL_ZOOM_WINDOW_FIXED_SIZE = (byte) 0x02;

const static int ENUM_PARAM_CALIBRATION_TYPE = 8;
const static byte CALIBRATION_LAT_LON = (byte) 0x01;
const static byte CALIBRATION_YAW_PITCH = (byte) 0x02;

const static int ENUM_PARAM_LIGHT_LEVEL = 9;
const static byte LIGHT_LEVEL_ERROR = (byte) 0x00;
const static byte LIGHT_LEVEL_NO_LIGHT = (byte) 0x01;
const static byte LIGHT_LEVEL_LOW_LIGHT = (byte) 0x02;
const static byte LIGHT_LEVEL_MID_LIGHT = (byte) 0x03;
const static byte LIGHT_LEVEL_HIGH_LIGHT = (byte) 0x04;

const static int ENUM_PARAM_SCENARIO = 10;
const static byte SCENARIO_1 = (byte) 0x01;
const static byte SCENARIO_2 = (byte) 0x02;
const static byte SCENARIO_3 = (byte) 0x03;
const static byte SCENARIO_4 = (byte) 0x04;

const static int ENUM_PARAM_FOV_LEVEL = 11; //
const static byte FOV_LEVEL_1 = (byte) 0x01;
const static byte FOV_LEVEL_2 = (byte) 0x02;
const static byte FOV_LEVEL_3 = (byte) 0x03;
const static byte FOV_LEVEL_4 = (byte) 0x04;
const static byte FOV_LEVEL_5 = (byte) 0x05;
const static byte FOV_LEVEL_6 = (byte) 0x06;

// Sarp active passive uyarı mesaji -- command mi parametre mi?
const static int ENUM_PARAM_LAST_BULLET_WARNING = 12;
const static byte PASSIVE_WARNING = (byte) 0x00;
const static byte ACTIVE_WARNING = (byte) 0x01;

const static int ENUM_PARAM_GAIN_LEVEL = 13; //
const static byte GAIN_LEVEL_1 = (byte) 0x01;
const static byte GAIN_LEVEL_2 = (byte) 0x02;
const static byte GAIN_LEVEL_3 = (byte) 0x03;
const static byte GAIN_LEVEL_4 = (byte) 0x04;
const static byte GAIN_LEVEL_5 = (byte) 0x05;
const static byte GAIN_LEVEL_6 = (byte) 0x06;

// ENUM COMMANDS
const static int ENUM_COMMAND_TRACK = 0;
const static byte TRACK_START = (byte) 0x01;
const static byte TRACK_STOP = (byte) 0x02;

const static int ENUM_COMMAND_DIGITAL_ZOOM = 1;
const static byte DIGITAL_ZOOM_START = (byte) 0x01;
const static byte DIGITAL_ZOOM_STOP = (byte) 0x02;

const static int ENUM_COMMAND_MENU_OVER_VIDEO = 2;
const static byte MENU_UP = (byte) 0x01;
const static byte MENU_DOWN = (byte) 0x02;
const static byte MENU_LEFT = (byte) 0x03;
const static byte MENU_RIGHT = (byte) 0x04;
const static byte MENU_MOUSE_PRESSED = (byte) 0x05;
const static byte MENU_MOUSE_RELEASED = (byte) 0x06;

const static int ENUM_COMMAND_LAZER_RANGE_FINDER = 3;
const static byte LAZER_POWER_ON = (byte) 0x01;
const static byte LAZER_POWER_OFF = (byte) 0x02;
const static byte LAZER_COMPUTE_RANGE = (byte) 0x03;

const static int ENUM_COMMAND_SYSTEM = 4;
const static byte SYSTEM_RESTART = (byte) 0x01;
const static byte SYSTEM_SHUT_DOWN = (byte) 0x02;
const static byte SYSTEM_NUC = (byte) 0x03;
const static byte SYSTEM_STOP = (byte) 0x04;

static inline void addcap(int64_t &c, int cap)
{
	c |= ((int64_t)1 << cap);
}
static inline void addbit(int32_t &v, int bit)
{
	v |= (1 << bit);
}

}

class KardelenAPIServer : public kaapi::CameraService::Service
{
public:
	KardelenAPIServer(PtzpDriver *ptzp, QString nodeType);

	static KardelenAPIServer * instance();
	void setMotionObjects(const std::vector<alarmGeneratorElement::TargetStruct> &v);

protected:
	KardelenAPIImpl *impl;


	// Service interface
public:
	grpc::Status GetPosition(grpc::ServerContext *, const google::protobuf::Empty *, kaapi::PosInfo *response) override;
	grpc::Status MoveAbsolute(grpc::ServerContext *, const kaapi::AbsoluteMoveParameters *request, kaapi::AbsoluteMoveResult *response) override;
	grpc::Status MoveRelative(grpc::ServerContext *, const kaapi::RelativeMoveParameters *request, kaapi::RelativeMoveResult *response) override;
	grpc::Status GetCameraStatus(grpc::ServerContext *, const kaapi::GetCameraStatusParameters *, kaapi::CameraStatus *response) override;
	grpc::Status UpdateCameraStatus(grpc::ServerContext *context, const kaapi::UpdateCameraStatusParameters *, kaapi::CameraStatus *) override;
	grpc::Status CommunicationChannel(grpc::ServerContext *, ::grpc::ServerReaderWriter<kaapi::CommRead, kaapi::CommWrite> *stream) override;
	grpc::Status SetCamera(grpc::ServerContext *, const kaapi::SetCameraRequest *request, kaapi::SetCameraResponse *response) override;
	grpc::Status GetNumericParameter(grpc::ServerContext *, const kaapi::GetNumericParameterRequest *request, kaapi::GetNumericParameterResponse *response) override;
	grpc::Status GetEnumParameter(grpc::ServerContext *, const kaapi::GetEnumParameterRequest *request, kaapi::GetEnumParameterResponse *response) override;
	grpc::Status SetNumericParameter(grpc::ServerContext *, const kaapi::SetNumericParameterRequest *request, kaapi::SetNumericParameterResponse *response) override;
	grpc::Status SetEnumParameter(grpc::ServerContext *, const kaapi::SetEnumParameterRequest *request, kaapi::SetEnumParameterResponse *response) override;
	grpc::Status SetEnumCommand(grpc::ServerContext *, const kaapi::SetEnumCommandRequest *request, kaapi::SetEnumCommandResponse *response) override;
	grpc::Status GetVersion(grpc::ServerContext *, const google::protobuf::Empty *, kaapi::ApiVersion *response) override;
	grpc::Status ScreenClick(grpc::ServerContext *, const kaapi::ClickParameter *request, google::protobuf::Empty *) override;
	grpc::Status SetMotionROI(grpc::ServerContext *context, const kaapi::MotionROIRequest *request, google::protobuf::Empty *) override;
	grpc::Status SetTrackWindow(grpc::ServerContext *, const kaapi::Rectangle *request, google::protobuf::Empty *) override;
	grpc::Status SetCalibration(grpc::ServerContext *, const kaapi::CalibrationRequest *request, google::protobuf::Empty *) override;
	grpc::Status SetAselChangeLocations(grpc::ServerContext *context, const kaapi::ListOfLocationInformation *request, google::protobuf::Empty *response);


protected:
	QMutex mutex;
	std::vector<alarmGeneratorElement::TargetStruct> lastMotionObjects;
	AlgorithmCommunication::RequestForAlgorithm req;
	AlgorithmCommunication::ResponseOfRequests resp;
};

#endif // KARDELENAPI_H
