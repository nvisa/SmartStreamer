#include "kardelenapi.h"
#include "proto/KardelenAPI.pb.h"
#include "algorithm/algorithmgrpcserver.h"

#include <ecl/ptzp/ptzphead.h>
#include <ecl/ptzp/ptzpdriver.h>

#include <QDebug>
#include <QThread>

#include <string>

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/channel.h>
#include <grpc++/create_channel.h>
#include <grpc++/client_context.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/credentials.h>
#include <grpc++/security/server_credentials.h>

using namespace kaapi;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::Status;
using grpc::StatusCode;

#define NOT_SUPP 100000

class KaapiThread : public QThread
{
public:
	KaapiThread(quint16 port, KardelenAPIServer *s)
	{
		servicePort = port;
		kaapi = s;
	}
	void run()
	{
		std::string ep(qPrintable(QString("0.0.0.0:%1").arg(servicePort)));
		ServerBuilder builder;
		builder.AddListeningPort(ep, grpc::InsecureServerCredentials());
		builder.RegisterService(kaapi);
		std::unique_ptr<Server> server(builder.BuildAndStart());
		server->Wait();
	}

protected:
	int servicePort;
	KardelenAPIServer *kaapi;
};

class KardelenAPIImpl
{
public:
	virtual int64_t getCapabilities() = 0;
	virtual void fillCameraStatus(kaapi::CameraStatus *response) = 0;
	virtual void setPosi(kaapi::PosInfo *posi) = 0;
	virtual void moveRelative(const kaapi::RelativeMoveParameters *request) = 0;
	virtual void moveAbsolute(const kaapi::AbsoluteMoveParameters *request) = 0;
	virtual void setCamera(int32_t type) = 0;
	virtual int32_t getEnumParameter(int index) = 0;
	virtual void setEnumParameter(int index, int32_t value) = 0;
	virtual void setEnumCommand(int index, int32_t value) = 0;
	virtual void screenClick(int x, int y, int action) = 0;

	virtual void getNumericParameterP(int index, double &value, int32_t bytes[3])
	{
		getNumericParameter(index, value, bytes);
	}

	int getMode()
	{
		return _mymode;
	}

	int setMode(int mode)
	{
		AlgorithmGrpcServer *papi = AlgorithmGrpcServer::instance();
		AlgorithmCommunication::RequestForAlgorithm req;
		grpc::ServerContext ctx;
		AlgorithmCommunication::ResponseOfRequests resp;

		if (mode == CONTROL_MODE_JOYSTICK) {
			/* we need to stop all algorithms */
			req.set_channel(0);
			req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm::MOTION);
			papi->StopAlgorithm(&ctx, &req, &resp);

			req.set_channel(1);
			req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm::TRACKING);
			papi->StopAlgorithm(&ctx, &req, &resp);
		} else if (mode == CONTROL_MODE_DETECTION) {
			req.set_channel(0);
			req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm::MOTION);
			papi->RunAlgorithm(&ctx, &req, &resp);
		} else if (mode == CONTROL_MODE_TRACK_WINDOW_SELECT) {
			/* nothing to do on our side in this mode, TODO: this may change for auto track */
		} else if (mode == CONTROL_MODE_TRACK_STARTED) {
			req.set_channel(1);
			req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm::TRACKING);
			papi->RunAlgorithm(&ctx, &req, &resp);
		} else if (mode == CONTROL_MODE_DIGITAL_ZOOM_STARTED) {
			/* TODO: handle digital zoom start */
		} else if (mode == CONTROL_MODE_DIGITAL_ZOOM_WINDOW_SELECT) {
			/* probably nothing to do on our side */
		}
		_mymode = mode;
		return 0;
	}
	int _mymode;

	virtual void setNumericParameterP(int index, double &value, int32_t bytes[3])
	{
		if (index == NUM_PARAM_YAW || index == NUM_PARAM_PITCH) {
			if (bytes[2] == VALUE_NO_CHANGE) {

			} else if (bytes[2] == VALUE_INCREASE) {
				if (index == NUM_PARAM_YAW)
					lastPanSpeed = value / 100.0;
				else if (index == NUM_PARAM_PITCH)
					lastTiltSpeed = -1 * value / 100.0;
				/* relative movement */
				kaapi::RelativeMoveParameters req;
				req.set_panspeed(lastPanSpeed);
				req.set_tiltspeed(lastTiltSpeed);
				req.set_zoomspeed(lastZoomSpeed);
				moveRelative(&req);
			} else if (bytes[2] == VALUE_DECREASE) {
				if (index == NUM_PARAM_YAW)
					lastPanSpeed = -1 * value / 100.0;
				else if (index == NUM_PARAM_PITCH)
					lastTiltSpeed = value / 100.0;
				/* relative movement */
				kaapi::RelativeMoveParameters req;
				req.set_panspeed(lastPanSpeed);
				req.set_tiltspeed(lastTiltSpeed);
				req.set_zoomspeed(lastZoomSpeed);
				moveRelative(&req);
			} else if (bytes[2] == VALUE_SET) {
				if (index == NUM_PARAM_YAW)
					lastPanPos = value / 100.0;
				else if (index == NUM_PARAM_PITCH)
					lastTiltPos = value / 100.0;
				/* goto preset or position */
				kaapi::AbsoluteMoveParameters req;
				req.set_panpos(lastPanPos);
				req.set_tiltpos(lastTiltPos);
				moveAbsolute(&req);
			}
		} else if (index == NUM_PARAM_FOV) {
			/* NOTE: zoom speed is no-op for falconeye */
			lastZoomSpeed = value;
			kaapi::RelativeMoveParameters req;
			req.set_panspeed(lastPanSpeed);
			req.set_tiltspeed(lastTiltSpeed);
			req.set_zoomspeed(lastZoomSpeed);
			moveRelative(&req);
			if (bytes[2] == VALUE_NO_CHANGE) {
			} else if (bytes[2] == VALUE_INCREASE) {
			} else if (bytes[2] == VALUE_DECREASE) {
			} else if (bytes[2] == VALUE_SET) {
			}
		} else
			setNumericParameter(index, value, bytes);
	}

	double lastPanSpeed;
	double lastTiltSpeed;
	double lastZoomSpeed;
	double lastPanPos;
	double lastTiltPos;
	PtzpDriver *ptzp;

protected:
	virtual void getNumericParameter(int index, double &value, int32_t bytes[3]) = 0;
	virtual void setNumericParameter(int index, double &value, int32_t bytes[3]) = 0;

	void addNumericParameter(int32_t &v, int32_t type, kaapi::CameraStatus *response)
	{
		addbit(v, type);
		double value;
		int32_t bytes[] {0, 0, 0};
		getNumericParameter(type, value, bytes);
		kaapi::NumericParameter *np = response->add_numericparameters();
		np->set_value(value);
		np->set_byte0(bytes[0]);
		np->set_byte1(bytes[1]);
		np->set_byte2(bytes[2]);
	}
	void addEnumParameter(int32_t &v, int32_t type, kaapi::CameraStatus *response)
	{
		addbit(v, type);
		response->add_enumparameters(getEnumParameter(type));
	}
};

class KardelenAPIFalconEyeImpl : public KardelenAPIImpl
{
public:
	KardelenAPIFalconEyeImpl()
	{
		setMode(CONTROL_MODE_JOYSTICK);
	}

	int64_t getCapabilities()
	{
		int64_t caps = 0;
		if (THERMAL) {
			addcap(caps, CAPABILITY_JOYSTICK_CONTROL);
			addcap(caps, CAPABILITY_DETECTION);
			addcap(caps, CAPABILITY_TRACKING);
			addcap(caps, CAPABILITY_ZOOM);
			addcap(caps, CAPABILITY_POLARITY);
			addcap(caps, CAPABILITY_PT);
			addcap(caps, CAPABILITY_ROI);
			addcap(caps, CAPABILITY_DAY_VIEW);
			addcap(caps, CAPABILITY_NIGHT_VIEW);
			addcap(caps, CAPABILITY_RANGE); //TODO: return sahingoz static value
			addcap(caps, CAPABILITY_MENU_OVER_VIDEO);
			addcap(caps, CAPABILITY_LAZER_RANGE_FINDER);
			addcap(caps, CAPABILITY_SHOW_HIDE_SEMBOLOGY);
			addcap(caps, CAPABILITY_AUTO_TRACK_DETECTION);
			addcap(caps, CAPABILITY_NUC);
			addcap(caps, CAPABILITY_DIGITAL_ZOOM);

			addcap(caps, CAPABILITY_MISSION_EXECUTION);
			addcap(caps, CAPABILITY_SENSITIVITY_ADJUSTMENT);
			addcap(caps, CAPABILITY_CALIBRATION);
			addcap(caps, CAPABILITY_HARD_CALIBRATION);
		} else {
			addcap(caps, CAPABILITY_JOYSTICK_CONTROL);
			addcap(caps, CAPABILITY_DETECTION);
			addcap(caps, CAPABILITY_TRACKING);
			addcap(caps, CAPABILITY_ZOOM);
			addcap(caps, CAPABILITY_FOCUS);
			addcap(caps, CAPABILITY_PT);
			addcap(caps, CAPABILITY_ROI);
			addcap(caps, CAPABILITY_DAY_VIEW);
			addcap(caps, CAPABILITY_NIGHT_VIEW);
			addcap(caps, CAPABILITY_RANGE); //TODO: return sahingoz static value
			addcap(caps, CAPABILITY_MENU_OVER_VIDEO);
			addcap(caps, CAPABILITY_LAZER_RANGE_FINDER);
			addcap(caps, CAPABILITY_SHOW_HIDE_SEMBOLOGY);
			addcap(caps, CAPABILITY_AUTO_TRACK_DETECTION);
			addcap(caps, CAPABILITY_DIGITAL_ZOOM);

			addcap(caps, CAPABILITY_MISSION_EXECUTION);
			addcap(caps, CAPABILITY_SENSITIVITY_ADJUSTMENT);
			addcap(caps, CAPABILITY_CALIBRATION);
			addcap(caps, CAPABILITY_HARD_CALIBRATION);
		}

		return caps;
	}

	void setPosi(kaapi::PosInfo *posi)
	{
		posi->set_panpos(ptzp->getHead(1)->getPanAngle());
		posi->set_tiltpos(ptzp->getHead(1)->getTiltAngle());
		posi->set_zoompos(ptzp->getHead(0)->getZoom());
		float fovh, fovv;
		ptzp->getHead(0)->getFOV(fovh, fovv);
		posi->set_fovh(fovh);
		posi->set_fovv(fovv);
	}

	void fillCameraStatus(kaapi::CameraStatus *response)
	{
		response->set_capabilities(getCapabilities());

		int32_t v = 0;

		/* numeric parameters */
//		addNumericParameter(v, NUM_PARAM_RANGE, response);
//		addNumericParameter(v, NUM_PARAM_HEIGHT, response);
		addNumericParameter(v, NUM_PARAM_FOV, response);
//		addNumericParameter(v, NUM_PARAM_FOCUS, response);
		addNumericParameter(v, NUM_PARAM_YAW, response);
		addNumericParameter(v, NUM_PARAM_PITCH, response);
		addNumericParameter(v, NUM_PARAM_HORIZONTAL_RES, response);
		addNumericParameter(v, NUM_PARAM_VERTICAL_RES, response);
		addNumericParameter(v, NUM_PARAM_ZOOM, response);
		addNumericParameter(v, NUM_PARAM_PREDEFINED_FOV_COUNT, response);
//		addNumericParameter(v, NUM_PARAM_HPF_GAIN, response);
//		addNumericParameter(v, NUM_PARAM_HPF_SPATIAL, response);
		response->set_numericparametersvector(v);

		/* enum parameters */
		v = 0;
		addEnumParameter(v, ENUM_PARAM_CAMERA_TYPE, response);
		addEnumParameter(v, ENUM_PARAM_OPERATIONAL_MODE, response);
		addEnumParameter(v, ENUM_PARAM_DETECTION_CREATION_MODE, response);
		addEnumParameter(v, ENUM_PARAM_POLARITY, response);
		addEnumParameter(v, ENUM_PARAM_SEMBOLOGY, response);
		addEnumParameter(v, ENUM_PARAM_FOV_LEVEL, response);
		response->set_enumparametersvector(v);
	}

	void moveRelative(const kaapi::RelativeMoveParameters *request)
	{
		/* speed is no-op for falcon-eye */
		if (request->zoomspeed() > 0)
			ptzp->getHead(0)->startZoomIn(0);
		else if (request->zoomspeed() < 0)
			ptzp->getHead(0)->startZoomOut(0);
		else
			ptzp->getHead(0)->stopZoom();
		ptzp->getHead(1)->panTiltAbs(request->panspeed() / 100.0, request->tiltspeed() / 100.0);
	}

	void moveAbsolute(const kaapi::AbsoluteMoveParameters *request)
	{
		ptzp->getHead(1)->panTiltGoPos(request->panpos(), request->tiltpos());
	}

	void setCamera(int32_t type)
	{
		if (map.isEmpty())
			map = ptzp->getHead(0)->getSettings();
		/* Kamera tipine göre görüntü değişimi (termal = 0, gündüz = 1) */
		if (type == TV)
			ptzp->getHead(0)->setProperty(5, 1);
		else
			ptzp->getHead(0)->setProperty(5, 0);
	}

	virtual void getNumericParameter(int index, double &value, int32_t bytes[3])
	{
		if (map.isEmpty())
			map = ptzp->getHead(0)->getSettings();

		value = 0;

		if (index == NUM_PARAM_FOV) {
			value = ptzp->getHead(0)->getProperty(map["fov_pos"].toUInt()); //TODO: convert to degrees
		}
		/* else if (index == NUM_PARAM_FOCUS) {
			value = ptzp->getHead(0)->getProperty(map["focus_pos_set"].toUInt());
		}
		*/
		else if (index == NUM_PARAM_YAW) {
			float pana = ptzp->getHead(1)->getPanAngle();
			if (pana > 180)
				value = pana - 360;
			else
				value = pana;
		} else if (index == NUM_PARAM_PITCH) {
			value = ptzp->getHead(1)->getTiltAngle();
		} else if (index == NUM_PARAM_HORIZONTAL_RES) {
			value = 720;
		} else if (index == NUM_PARAM_VERTICAL_RES) {
			value = 576;
		} else if (index == NUM_PARAM_ZOOM) {
			if(ptzp->getHead(0)->getProperty(2) == 0)
				value = 0; //TODO: normalize
			else if (ptzp->getHead(0)->getProperty(2) == 1){
				if (ptzp->getHead(0)->getProperty(3))
					value = 66;
				else
					value = 83;
			}
			else if (ptzp->getHead(0)->getProperty(2) == 2)
				value = 100;
		}
		/* Toplam fov sayısı(wide, middle, narrow) */
		else if(index == NUM_PARAM_PREDEFINED_FOV_COUNT)
			value = 3;
			/*  Şartname içeriğinde olmadığı için kaldırıldı
		else if (index == NUM_PARAM_HPF_GAIN) {
			value = ptzp->getHead(0)->getProperty(map["hf_sigma_coeff"].toUInt());
		} else if (index == NUM_PARAM_HPF_SPATIAL) {
			value = ptzp->getHead(0)->getProperty(map["hf_filter_std"].toUInt());
		}
		 else if (index == NUM_PARAM_RANGE || index == NUM_PARAM_HEIGHT) {
			QString lref = map["laser_reflections"].toString();
			if (lref.size()) {
//				 TODO: how to report multiple detections
				QString ref0 = lref.split(";").first();
				QStringList flds = ref0.split(",");
				if (value == NUM_PARAM_RANGE)
					value = flds[0].toInt();
				else
					value = flds[1].toInt();
//				 TODO: we have laser range but no laser height?
			}
		}
*/		else {
			value = 100000;
		}
	}

	virtual int32_t getEnumParameter(int index)
	{
		if (map.isEmpty())
			map = ptzp->getHead(0)->getSettings();

		if (index == ENUM_PARAM_CAMERA_TYPE) {
			if (ptzp->getHead(0)->getProperty(3))
				return TV;
			else
				return THERMAL;
		}
		if (index == ENUM_PARAM_OPERATIONAL_MODE)
			return getMode();
		if (index == ENUM_PARAM_DETECTION_CREATION_MODE)
			return DETECTION_OPEN_MODE;
		if (index == ENUM_PARAM_POLARITY) {
			if (ptzp->getHead(0)->getProperty(map["polarity"].toInt()))
				return BLACK_HOT;
			else
				return WHITE_HOT;
		}
		if (index == ENUM_PARAM_SEMBOLOGY) {
			if (ptzp->getHead(0)->getProperty(map["symbology"].toInt()))
				return SYMBOLOGY_ON;
			else
				return SYMBOLOGY_OFF;
		}
		if (index == ENUM_PARAM_FOV_LEVEL){
			return (ptzp->getHead(0)->getProperty(2) + 1);
		}
		if (index == ENUM_PARAM_CALIBRATION_TYPE) {
			return CALIBRATION_YAW_PITCH;
		}

		/* API wants this */
		return -1;
	}

	virtual void setNumericParameter(int index, double &value, int32_t bytes[3])
	{
		if (index == NUM_PARAM_FOV)
			ptzp->getHead(0)->setProperty("fov_pos", value); /* TODO: what is this? an enum */
		else if (index == NUM_PARAM_FOCUS)
			ptzp->getHead(0)->setProperty("focus_in", value);
	}

	virtual void setEnumParameter(int index, int32_t value)
	{
		qDebug() << index << value;

		if (index == ENUM_PARAM_CAMERA_TYPE) {
			if (value == TV)
				ptzp->getHead(0)->setProperty("choose_cam", 1);
			else if (value == THERMAL)
				ptzp->getHead(0)->setProperty("choose_cam", 0);
		} else if (index == ENUM_PARAM_POLARITY) {
			if (value == (int)BLACK_HOT)
				ptzp->getHead(0)->setProperty(7, 1);
			else if (value == WHITE_HOT)
				ptzp->getHead(0)->setProperty(7, 0);
		} else if (index == ENUM_PARAM_SEMBOLOGY){
			if (value == SYMBOLOGY_ON)
				ptzp->getHead(0)->setProperty(map["symbology"].toInt(), 1);
			else if (value == SYMBOLOGY_OFF)
				ptzp->getHead(0)->setProperty(map["symbology"].toInt(), 0);
		} else if (index == ENUM_PARAM_OPERATIONAL_MODE)
			setMode(value);
		else if (index == ENUM_PARAM_FOV_LEVEL)
			ptzp->getHead(0)->setProperty(4, value -1);
	}

	virtual void setEnumCommand(int index, int32_t value)
	{
		// TODO: implement digital zoom
		// TODO: cache all enum and numeric values
		qDebug() << index << value;
		/* digital zoom operational mode digital window select'e bağlanacak */
		if (index == ENUM_COMMAND_DIGITAL_ZOOM){
			if (value == DIGITAL_ZOOM_START )
				ptzp->getHead(0)->setProperty(6, 1);
			else if(value == DIGITAL_ZOOM_STOP)
				ptzp->getHead(0)->setProperty(6, 0);
		}
		else if (index == ENUM_COMMAND_SYSTEM) {
			if (value == SYSTEM_NUC)
				ptzp->getHead(0)->setProperty(17,8);
		}

	}

	double BUTTON_HEIGHT 	= 0.108; //0.112
	double BUTTON_WIDTH 	= 0.158; //0.158
	double FIRST_BUTTON_UP_LEFT_X = 0.026; //0.026
	double FIRST_BUTTON_UP_LEFT_Y = 0.246; //0.246
	double SECOND_BUTTON_UP_LEFT_X = 0.026; //0.026
	double SECOND_BUTTON_UP_LEFT_Y = 0.424; //0.555
	double THIRD_BUTTON_UP_LEFT_X = 0.026; //0.026
	double THIRD_BUTTON_UP_LEFT_Y = 0.602; //0.710
	double FOURTH_BUTTON_UP_LEFT_X = 0.793; //0.793
	double FOURTH_BUTTON_UP_LEFT_Y = 0.246; //0.246
	double FIFTH_BUTTON_UP_LEFT_X = 0.793; //0.793
	double FIFTH_BUTTON_UP_LEFT_Y = 0.424;//0.555
	double SIXTH_BUTTON_UP_LEFT_X = 0.793; //0.793
	double SIXTH_BUTTON_UP_LEFT_Y = 0.602; //0.710

	double ROUTING_LEFT_X1 = 0.815; //0.815
	double ROUTING_LEFT_X2 = 0.84; //0.840
	double ROUTING_RIGHT_X1 = 0.86; //0.861
	double ROUTING_RIGHT_X2 = 0.887; //0.8875
	double ROUTING_RIGHT_LEFT_Y1 = 0.871;  //0.871
	double ROUTING_RIGHT_LEFT_Y2 = 0.9;  //0.9
	double ROUTING_UP_DOWN_X1 = 0.84; //0.840
	double ROUTING_UP_DOWN_X2 = 0.887; //0.0887
	double ROUTING_UP_Y1 = 0.87; //0.871
	double ROUTING_UP_Y2 = 0.84; //0.842
	double ROUTING_DOWN_Y1 = 0.93; //0.930
	double ROUTING_DOWN_Y2 = 0.90; //0.901

	double ROUTING_AREA_TOP 	= 0.84; //0.840
	double ROUTING_AREA_BOTTOM = 0.93; //0.930
	int L0 = 0;
	int L1 = 1;
	int L2 = 2;
	int R0 = 3;
	int R1 = 4;
	int R2 = 5;
	int UP = 6;
	int DOWN = 7;
	int LEFT = 8;
	int RIGHT = 9;
	virtual void screenClick(int x, int y, int action)
	{
		double ratioX = x / 720.0;
		double ratioY = y / 576.0;

		if(ratioY >= FIRST_BUTTON_UP_LEFT_Y && ratioY <= (FIRST_BUTTON_UP_LEFT_Y + BUTTON_HEIGHT)) {
			if(ratioX >= FIRST_BUTTON_UP_LEFT_X && ratioX <= (FIRST_BUTTON_UP_LEFT_X + BUTTON_WIDTH)){
				buttonClick(L0, action);
			} else if(ratioX >= FOURTH_BUTTON_UP_LEFT_X && ratioX <= (FOURTH_BUTTON_UP_LEFT_X + BUTTON_WIDTH)) {
				buttonClick(R0, action);
			}
		} else if(ratioY >= SECOND_BUTTON_UP_LEFT_Y && ratioY <= (SECOND_BUTTON_UP_LEFT_Y + BUTTON_HEIGHT)) {
			if(ratioX >= SECOND_BUTTON_UP_LEFT_X && ratioX <= (SECOND_BUTTON_UP_LEFT_X + BUTTON_WIDTH)){
				buttonClick(L1, action);
			} else if(ratioX >= FIFTH_BUTTON_UP_LEFT_X && ratioX <= (FIFTH_BUTTON_UP_LEFT_X + BUTTON_WIDTH)) {
				buttonClick(R1, action);
			}
		} else if(ratioY >= THIRD_BUTTON_UP_LEFT_Y && ratioY <= (THIRD_BUTTON_UP_LEFT_Y + BUTTON_HEIGHT)) {
			if(ratioX >= THIRD_BUTTON_UP_LEFT_X && ratioX <= (THIRD_BUTTON_UP_LEFT_X + BUTTON_WIDTH)){
				buttonClick(L2, action);
			} else if(ratioX >= SIXTH_BUTTON_UP_LEFT_X && ratioX <= (SIXTH_BUTTON_UP_LEFT_X + BUTTON_WIDTH)) {
				buttonClick(R2, action);
			}
		} else if(ratioY >= ROUTING_AREA_TOP && ratioY <= ROUTING_AREA_BOTTOM){
			if(ratioX >= ROUTING_LEFT_X1 && ratioX <= ROUTING_LEFT_X2 && ratioY >= ROUTING_RIGHT_LEFT_Y1 && ratioY<= ROUTING_RIGHT_LEFT_Y2) {
				buttonClick(LEFT, action);
			} else if(ratioX >= ROUTING_RIGHT_X1 && ratioX <= ROUTING_RIGHT_X2 && ratioY >= ROUTING_RIGHT_LEFT_Y1 && ratioY<= ROUTING_RIGHT_LEFT_Y2) {
				buttonClick(RIGHT, action);
			} else if(ratioY >= ROUTING_DOWN_Y2 && ratioY <= ROUTING_DOWN_Y1 && ratioX >= ROUTING_UP_DOWN_X1 && ratioX <= ROUTING_UP_DOWN_X2) {
				buttonClick(DOWN, action);
			} else if(ratioY >= ROUTING_UP_Y2 && ratioY <= ROUTING_UP_Y1 && ratioX >= ROUTING_UP_DOWN_X1 && ratioX <= ROUTING_UP_DOWN_X2) {
				buttonClick(UP, action);
			}
		}
	}

	void buttonClick(int b, int action)
	{
		if (action == MENU_MOUSE_PRESSED)
			ptzp->getHead(0)->setProperty(35, b);
		else if (action == MENU_MOUSE_RELEASED)
			ptzp->getHead(0)->setProperty(36, b);
	}

	QVariantMap map;
};

class KardelenAPIYamgozImpl : public KardelenAPIImpl
{
public:
	KardelenAPIYamgozImpl()
	{

	}

	int64_t getCapabilities()
	{
		int64_t caps = 0;
		if (THERMAL){
			addcap(caps, CAPABILITY_POLARITY);
			addcap(caps, CAPABILITY_NUC);
			addcap(caps, CAPABILITY_DAY_VIEW);
			addcap(caps, CAPABILITY_NIGHT_VIEW);
		} else {
			addcap(caps, CAPABILITY_DAY_VIEW);
			addcap(caps, CAPABILITY_NIGHT_VIEW);
		}
		/* todo: algorithm capabilities */

		return caps;
	}

	void setPosi(kaapi::PosInfo *posi)
	{
		/* todo for triple head */
	}

	void fillCameraStatus(kaapi::CameraStatus *response)
	{
		response->set_capabilities(getCapabilities());

		int32_t v = 0;

		addEnumParameter(v, ENUM_PARAM_CAMERA_TYPE, response);
		addEnumParameter(v, ENUM_PARAM_POLARITY, response);

		/* todo addEnumCommand */


	}

	void moveRelative(const kaapi::RelativeMoveParameters *request)
	{

	}

	void moveAbsolute(const kaapi::AbsoluteMoveParameters *request)
	{

	}

	void setCamera(int32_t type)
	{
		/* TODO: handle camera type switch */
		/* Kamera tipine göre görüntü değişimi (termal = 0, gündüz = 1) */
		if (type == TV) {
			ptzp->getHead(0)->setProperty(4, 1);
			ptzp->getHead(1)->setProperty(4, 1);
			ptzp->getHead(2)->setProperty(4, 1);
		}
		else if (type == THERMAL) {
			ptzp->getHead(0)->setProperty(4, 0);
			ptzp->getHead(1)->setProperty(4, 0);
			ptzp->getHead(2)->setProperty(4, 0);
		}

	}

	virtual void getNumericParameter(int index, double &value, int32_t bytes[3])
	{
		value = 100000;
	}

	virtual int32_t getEnumParameter(int index)
	{
		if (map.isEmpty())
			map = ptzp->getHead(0)->getSettings();

		/* TODO: report correct camera type */ // ---> done
		/* TODO: report correct thermal polarity */ // ---> done
		if (index == ENUM_PARAM_CAMERA_TYPE) {
			if (ptzp->getHead(0)->getProperty(3))
				return TV;
			else
				return THERMAL;
		}

		if (index == ENUM_PARAM_POLARITY) {
			if (ptzp->getHead(0)->getProperty(2))
				return WHITE_HOT;
			else
				return BLACK_HOT;
		}

		return -1;
	}

	virtual void setNumericParameter(int index, double &value, int32_t bytes[3])
	{

	}

	virtual void setEnumParameter(int index, int32_t value)
	{
		if (index == ENUM_PARAM_POLARITY) {
			if (value == BLACK_HOT){
				ptzp->getHead(0)->setProperty(2, 0);
				ptzp->getHead(1)->setProperty(2, 0);
				ptzp->getHead(2)->setProperty(2, 0);
			}
			else if (value == WHITE_HOT){
				ptzp->getHead(0)->setProperty(2, 1);
				ptzp->getHead(1)->setProperty(2, 1);
				ptzp->getHead(2)->setProperty(2, 1);
			}
		} else if (index == ENUM_PARAM_OPERATIONAL_MODE)
			setMode(value);
	}

	virtual void setEnumCommand(int index, int32_t value)
	{
		/* todo: enum commands (nuc and digital zoom) */
		if (index == ENUM_COMMAND_SYSTEM) {
			if (value == SYSTEM_NUC){
				ptzp->getHead(0)->setProperty(3, 8);
				ptzp->getHead(1)->setProperty(3, 8);
				ptzp->getHead(2)->setProperty(3, 8);
			}
		}
	}

	void screenClick(int x, int y, int action)
	{

	}

	QVariantMap map;
};

KardelenAPIServer::KardelenAPIServer(PtzpDriver *ptzp, QString nodeType)
{
	KaapiThread *grpcServ = new KaapiThread(50060, this);
	grpcServ->start();
	if (nodeType == "kayi")
		impl = new KardelenAPIFalconEyeImpl;
	else if (nodeType == "yamgoz")
		impl = new KardelenAPIYamgozImpl;
	impl->ptzp = ptzp;
}

grpc::Status KardelenAPIServer::GetPosition(grpc::ServerContext *, const google::protobuf::Empty *, kaapi::PosInfo *response)
{
	impl->setPosi(response);
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::MoveAbsolute(grpc::ServerContext *, const kaapi::AbsoluteMoveParameters *request, kaapi::AbsoluteMoveResult *response)
{
	impl->moveAbsolute(request);
	impl->setPosi(response->mutable_posinfo());
	response->set_completionstatus(kaapi::AbsoluteMoveResult_Completion_COMPLETED);
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::MoveRelative(grpc::ServerContext *, const kaapi::RelativeMoveParameters *request, kaapi::RelativeMoveResult *response)
{
	kaapi::PosInfo *posi = response->mutable_posinfo();
	impl->moveRelative(request);
	impl->setPosi(posi);
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::GetCameraStatus(grpc::ServerContext *, const kaapi::GetCameraStatusParameters *, kaapi::CameraStatus *response)
{
	impl->fillCameraStatus(response);
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::UpdateCameraStatus(grpc::ServerContext *context, const kaapi::UpdateCameraStatusParameters *, kaapi::CameraStatus *)
{
	return grpc::Status::CANCELLED;
}

grpc::Status KardelenAPIServer::CommunicationChannel(grpc::ServerContext *, ::grpc::ServerReaderWriter<kaapi::CommRead, kaapi::CommWrite> *stream)
{
	while (1) {
		kaapi::CommWrite msgw;
		stream->Read(&msgw);

		/* TODO: handle actions */
		if (msgw.action() == kaapi::CommWrite_Action_MOVE_ABS) {

		} else if (msgw.action() == kaapi::CommWrite_Action_MOVE_ABS) {

		} else if (msgw.action() == kaapi::CommWrite_Action_MOVE_REL) {

		} else if (msgw.action() == kaapi::CommWrite_Action_UPDATE) {

		} else if (msgw.action() == kaapi::CommWrite_Action_SET_CAMERA) {

		}

		kaapi::CommRead msgr;
		impl->setPosi(msgr.mutable_posinfo());
		impl->fillCameraStatus(msgr.mutable_status());
		stream->Write(msgr);
	}

	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::SetCamera(grpc::ServerContext *, const SetCameraRequest *request, SetCameraResponse *response)
{
	impl->setCamera(request->cameratype());
	response->set_capabilities(impl->getCapabilities());
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::GetNumericParameter(grpc::ServerContext *, const GetNumericParameterRequest *request, GetNumericParameterResponse *response)
{
	double value = 0;
	int32_t bytes[] = {0, 0, 0};
	impl->getNumericParameterP(request->index(), value, bytes);
	response->mutable_value()->set_value(value);
	response->mutable_value()->set_byte0(bytes[0]);
	response->mutable_value()->set_byte1(bytes[1]);
	response->mutable_value()->set_byte2(bytes[2]);
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::GetEnumParameter(grpc::ServerContext *, const GetEnumParameterRequest *request, GetEnumParameterResponse *response)
{
	response->set_value(impl->getEnumParameter(request->index()));
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::SetNumericParameter(grpc::ServerContext *, const SetNumericParameterRequest *request, SetNumericParameterResponse *response)
{
	int32_t index = request->index();
	double value = request->value().value();
	int32_t bytes[] = {request->value().byte0(), request->value().byte1(), request->value().byte2()};
	impl->setNumericParameterP(index, value, bytes);

	/* return old parameter */
	impl->getNumericParameterP(index, value, bytes);
	response->mutable_value()->set_value(value);
	response->mutable_value()->set_byte0(bytes[0]);
	response->mutable_value()->set_byte1(bytes[1]);
	response->mutable_value()->set_byte2(bytes[2]);

	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::SetEnumParameter(grpc::ServerContext *, const SetEnumParameterRequest *request, SetEnumParameterResponse *response)
{
	impl->setEnumParameter(request->index(), request->value());
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::SetEnumCommand(grpc::ServerContext *, const SetEnumCommandRequest *request, SetEnumCommandResponse *response)
{
	impl->setEnumCommand(request->index(), request->value());
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::GetVersion(grpc::ServerContext *, const google::protobuf::Empty *, ApiVersion *response)
{
	response->set_date(__DATE__);
	response->set_time(__TIME__);
	response->set_version("1.0.5");
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::ScreenClick(grpc::ServerContext *, const ClickParameter *request, google::protobuf::Empty *)
{
	impl->screenClick(request->pt().x(), request->pt().y(), request->value());
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::SetMotionROI(grpc::ServerContext *, const MotionROIRequest *request, google::protobuf::Empty *)
{
	kaapi::Polygon r = request->roi();
	for (int i = 0; i < r.points_size(); i++) {
		kaapi::Point p = r.points(i);
		qDebug() << i << p.x() << p.y();
	}

	kaapi::Rectangle far = request->far();
	kaapi::Rectangle close = request->close();

	AlgorithmGrpcServer *papi = AlgorithmGrpcServer::instance();
	AlgorithmCommunication::RequestForAlgorithm req;
	req.set_channel(0);
	req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm::MOTION);
	AlgorithmCommunication::MotionParameters* mpars = req.mutable_motionparam();
	mpars->set_settingchoice(AlgorithmCommunication::MotionParameters::BOTH);
	auto roilist = mpars->mutable_roilist();
	auto poly = roilist->add_polygon();
	for (int i = 0; i < r.points_size(); i++) {
		kaapi::Point p = r.points(i);
		auto pt = poly->add_points();
		pt->set_x(p.x());
		pt->set_y(p.y());
	}
	auto rect = roilist->mutable_rect1();
	auto pt = rect->mutable_upperleft();
	pt->set_x(far.topleft().x());
	pt->set_y(far.topleft().y());
	pt = rect->mutable_bottomright();
	pt->set_x(far.topleft().x() + far.width());
	pt->set_y(far.topleft().y() + far.height());

	rect = roilist->mutable_rect2();
	pt = rect->mutable_upperleft();
	pt->set_x(close.topleft().x());
	pt->set_y(close.topleft().y());
	pt = rect->mutable_bottomright();
	pt->set_x(close.topleft().x() + close.width());
	pt->set_y(close.topleft().y() + close.height());
	grpc::ServerContext ctx;
	AlgorithmCommunication::ResponseOfRequests resp;
	papi->SetAlgorithmParameters(&ctx, &req, &resp);

	qDebug() << "far" << far.topleft().x() << far.topleft().y() << far.width() << far.height();
	qDebug() << "close" << close.topleft().x() << close.topleft().y() << close.width() << close.height();

	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::SetTrackWindow(grpc::ServerContext *, const Rectangle *request, google::protobuf::Empty *)
{
	qDebug() << "track" << request->topleft().x() << request->topleft().y() << request->width() << request->height();
	// move to CONTROL_MODE_TRACK_WINDOW_SELECT, then move to CONTROL_MODE_TRACK_STARTED, return to CONTROL_MODE_TRACK_WINDOW_SELECT

	AlgorithmGrpcServer *papi = AlgorithmGrpcServer::instance();
	AlgorithmCommunication::RequestForAlgorithm req;
	grpc::ServerContext ctx;
	AlgorithmCommunication::ResponseOfRequests resp;

	req.set_channel(1);
	req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm::TRACKING);
	auto param = req.mutable_trackparam();
	param->set_tracktype(AlgorithmCommunication::TrackParameters::MANUAL);
	auto obj = param->mutable_target();
	obj->set_point_x(request->topleft().x());
	obj->set_point_y(request->topleft().y());
	obj->set_width(request->width());
	obj->set_height(request->height());
	papi->SetAlgorithmParameters(&ctx, &req, &resp);

	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::SetCalibration(grpc::ServerContext *, const CalibrationRequest *request, google::protobuf::Empty *)
{
	qDebug() << request->value1() << request->value2() << request->mode();
	return grpc::Status::OK;
}
