#include "kardelenapi.h"
#include "proto/KardelenAPI.pb.h"

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
	virtual void setCamera(int32_t type) = 0;
	virtual int32_t getEnumParameter(int index) = 0;
	virtual void setEnumParameter(int index, int32_t value) = 0;
	virtual void setEnumCommand(int index, int32_t value) = 0;

	virtual void getNumericParameterP(int index, double &value, int32_t bytes[3])
	{
		getNumericParameter(index, value, bytes);
	}

	virtual void setNumericParameterP(int index, double &value, int32_t bytes[3])
	{

		if (index == NUM_PARAM_YAW || index == NUM_PARAM_PITCH) {
			if (bytes[2] == VALUE_NO_CHANGE) {

			} else if (bytes[2] == VALUE_INCREASE) {
				if (index == NUM_PARAM_YAW)
					lastPanSpeed = value / 100.0;
				else if (index == NUM_PARAM_PITCH)
					lastTiltSpeed = value / 100.0;
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
					lastTiltSpeed = -1 * value / 100.0;
				/* relative movement */
				kaapi::RelativeMoveParameters req;
				req.set_panspeed(lastPanSpeed);
				req.set_tiltspeed(lastTiltSpeed);
				req.set_zoomspeed(lastZoomSpeed);
				moveRelative(&req);
			} else if (bytes[2] == 0x3) {
				/* goto preset or position */
			}
		} else
			setNumericParameter(index, value, bytes);
	}

	double lastPanSpeed;
	double lastTiltSpeed;
	double lastZoomSpeed;
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
		cameraType = 0;
		opMode = CONTROL_MODE_JOYSTICK;
	}

	int64_t getCapabilities()
	{
		int64_t caps = 0;
		addcap(caps, CAPABILITY_JOYSTICK_CONTROL);
		addcap(caps, CAPABILITY_DETECTION);
		addcap(caps, CAPABILITY_TRACKING);
		addcap(caps, CAPABILITY_ZOOM);
		addcap(caps, CAPABILITY_FOCUS);
		addcap(caps, CAPABILITY_POLARITY);
		addcap(caps, CAPABILITY_PT);
		addcap(caps, CAPABILITY_ROI);
		addcap(caps, CAPABILITY_DAY_VIEW);
		addcap(caps, CAPABILITY_NIGHT_VIEW);
		addcap(caps, CAPABILITY_RANGE);
		addcap(caps, CAPABILITY_MENU_OVER_VIDEO);
		addcap(caps, CAPABILITY_LAZER_RANGE_FINDER);
		addcap(caps, CAPABILITY_SHOW_HIDE_SEMBOLOGY);
		addcap(caps, CAPABILITY_SHUT_DOWN);
		addcap(caps, CAPABILITY_RESTART);
		addcap(caps, CAPABILITY_AUTO_TRACK_WINDOW);
		addcap(caps, CAPABILITY_AUTO_TRACK_DETECTION);
		addcap(caps, CAPABILITY_NUC);
		addcap(caps, CAPABILITY_HPF_GAIN);
		addcap(caps, CAPABILITY_HPF_SPATIAL);
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
		addNumericParameter(v, NUM_PARAM_RANGE, response);
		addNumericParameter(v, NUM_PARAM_HEIGHT, response);
		addNumericParameter(v, NUM_PARAM_FOV, response);
		addNumericParameter(v, NUM_PARAM_FOCUS, response);
		addNumericParameter(v, NUM_PARAM_YAW, response);
		addNumericParameter(v, NUM_PARAM_PITCH, response);
		addNumericParameter(v, NUM_PARAM_HORIZONTAL_RES, response);
		addNumericParameter(v, NUM_PARAM_VERTICAL_RES, response);
		addNumericParameter(v, NUM_PARAM_ZOOM, response);
		addNumericParameter(v, NUM_PARAM_HPF_GAIN, response);
		addNumericParameter(v, NUM_PARAM_HPF_SPATIAL, response);
		response->set_numericparametersvector(v);

		/* enum parameters */
		v = 0;
		addEnumParameter(v, ENUM_PARAM_CAMERA_TYPE, response);
		addEnumParameter(v, ENUM_PARAM_OPERATIONAL_MODE, response);
		addEnumParameter(v, ENUM_PARAM_DETECTION_CREATION_MODE, response);
		addEnumParameter(v, ENUM_PARAM_POLARITY, response);
		addEnumParameter(v, ENUM_PARAM_SEMBOLOGY, response);
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
		ptzp->getHead(1)->panTiltAbs(request->panspeed(), request->tiltspeed());
	}

	void setCamera(int32_t type)
	{
		/* TODO: handle camera type switch */
		/* Kamera tipine göre görüntü değişimi (termal = 0, gündüz = 1) */
		cameraType = type;
		ptzp->getHead(0)->setProperty("choose_cam", cameraType);
	}

	virtual void getNumericParameter(int index, double &value, int32_t bytes[3])
	{
		if (map.isEmpty())
			map = ptzp->getHead(0)->getSettings();

		value = 0;

		if (index == NUM_PARAM_FOV) {
			value = ptzp->getHead(0)->getProperty(map["fov_pos"].toUInt());
		} else if (index == NUM_PARAM_FOCUS) {
			value = ptzp->getHead(0)->getProperty(map["focus_pos_set"].toUInt());
		} else if (index == NUM_PARAM_YAW) {
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
			value = ptzp->getHead(0)->getProperty(map["fov_pos"].toUInt());
		} else if (index == NUM_PARAM_HPF_GAIN) {
			value = ptzp->getHead(0)->getProperty(map["hf_sigma_coeff"].toUInt());
		} else if (index == NUM_PARAM_HPF_SPATIAL) {
			value = ptzp->getHead(0)->getProperty(map["hf_filter_std"].toUInt());
		} else if (index == NUM_PARAM_RANGE || index == NUM_PARAM_HEIGHT) {
			QString lref = map["laser_reflections"].toString();
			if (lref.size()) {
				/* TODO: how to report multiple detections */
				QString ref0 = lref.split(";").first();
				QStringList flds = ref0.split(",");
				if (value == NUM_PARAM_RANGE)
					value = flds[0].toInt();
				else
					value = flds[1].toInt();
				/* TODO: we have laser range but no laser height? */
			}
		}
	}

	virtual int32_t getEnumParameter(int index)
	{
		if (map.isEmpty())
			map = ptzp->getHead(0)->getSettings();

		/* TODO: report correct camera type */ // ---> done
		/* TODO: report correct camera op mode */
		/* TODO: report correct detection creation mode */
		/* TODO: report correct thermal polarity */ // ---> done
		/* TODO: report correct symbology */ // ---> done
		if (index == ENUM_PARAM_CAMERA_TYPE) {
			if (ptzp->getHead(0)->getProperty("choose_cam").toInt())
				return TV;
			else
				return THERMAL;
		}
		if (index == ENUM_PARAM_OPERATIONAL_MODE)
			return opMode;
		if (index == ENUM_PARAM_DETECTION_CREATION_MODE)
			return DETECTION_OPEN_MODE;
		if (index == ENUM_PARAM_POLARITY) {
			if (ptzp->getHead(0)->getProperty("polarity").toInt())
				return BLACK_HOT;
			else
				return WHITE_HOT;
		}
		if (index == ENUM_PARAM_SEMBOLOGY) {
			if (ptzp->getHead(0)->getProperty("symbology").toInt())
				return SYMBOLOGY_ON;
			else
				return SYMBOLOGY_OFF;
		}

		/* API wants this */
		return -1;
	}

	virtual void setNumericParameter(int index, double &value, int32_t bytes[3])
	{
		if (index == NUM_PARAM_FOV)
			ptzp->getHead(0)->setProperty("fov_pos", value);
		else if (index == NUM_PARAM_FOCUS)
			ptzp->getHead(0)->setProperty("focus_in", value);
	}

	virtual void setEnumParameter(int index, int32_t value)
	{
		if (index == ENUM_PARAM_CAMERA_TYPE) {
			if (value == TV)
				ptzp->getHead(0)->setProperty("choose_cam", 1);
			else if (value == THERMAL)
				ptzp->getHead(0)->setProperty("choose_cam", 0);
		} else if (index == ENUM_PARAM_POLARITY) {
			if (value == BLACK_HOT)
				ptzp->getHead(0)->setProperty("choose_cam", 1);
			else if (value == WHITE_HOT)
				ptzp->getHead(0)->setProperty("choose_cam", 0);
		} else if (index == ENUM_PARAM_SEMBOLOGY){
			if (value == SYMBOLOGY_ON)
				ptzp->getHead(0)->setProperty("symbology", 1);
			else if (value == SYMBOLOGY_OFF)
				ptzp->getHead(0)->setProperty("symbology", 0);
		} else if (index == ENUM_PARAM_OPERATIONAL_MODE)
			opMode = value;
	}

	virtual void setEnumCommand(int index, int32_t value)
	{
	}

	int cameraType;
	int opMode;
	QVariantMap map;
};

KardelenAPIServer::KardelenAPIServer(PtzpDriver *ptzp, QString nodeType)
{
	KaapiThread *grpcServ = new KaapiThread(50060, this);
	grpcServ->start();
	if (nodeType == "kayi")
		impl = new KardelenAPIFalconEyeImpl;
	else
		Q_ASSERT(0);
	impl->ptzp = ptzp;
}

grpc::Status KardelenAPIServer::GetPosition(grpc::ServerContext *, const google::protobuf::Empty *, kaapi::PosInfo *response)
{
	impl->setPosi(response);
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::MoveAbsolute(grpc::ServerContext *, const kaapi::AbsoluteMoveParameters *request, kaapi::AbsoluteMoveResult *response)
{
	response->set_completionstatus(kaapi::AbsoluteMoveResult_Completion_COMPLETED);
	impl->setPosi(response->mutable_posinfo());
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
	impl->setEnumParameter(request->index(), response->value());
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::SetEnumCommand(grpc::ServerContext *, const SetEnumCommandRequest *request, SetEnumCommandResponse *response)
{
	impl->setEnumCommand(request->index(), response->value());
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::GetVersion(grpc::ServerContext *, const google::protobuf::Empty *, ApiVersion *response)
{
	response->set_date(__DATE__);
	response->set_time(__TIME__);
	response->set_version("1.0.3");
	return grpc::Status::OK;
}
