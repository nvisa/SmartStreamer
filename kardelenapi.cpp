#include "kardelenapi.h"
#include "proto/KardelenAPI.pb.h"

#include <ecl/ptzp/ptzphead.h>
#include <ecl/ptzp/ptzpdriver.h>

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

KardelenAPIServer::KardelenAPIServer(PtzpDriver *ptzp)
{
	KaapiThread *grpcServ = new KaapiThread(50060, this);
	grpcServ->start();
	this->ptzp = ptzp;
}

void KardelenAPIServer::setPosi(kaapi::PosInfo *posi)
{
	posi->set_panpos(ptzp->getHead(1)->getPanAngle());
	posi->set_tiltpos(ptzp->getHead(1)->getTiltAngle());
	posi->set_zoompos(ptzp->getHead(0)->getZoom());
	posi->set_fovh(0);
	posi->set_fovv(0);
}

void KardelenAPIServer::fillCameraStatus(kaapi::CameraStatus *response)
{
	response->set_capabilities(getCapabilities());

	int32_t v = 0;
	QVariantMap map = ptzp->getHead(0)->getSettings();
	kaapi::NumericParameter *np;
	float pana = ptzp->getHead(1)->getPanAngle();
	float tilta = ptzp->getHead(1)->getTiltAngle();

	QString lref = map["laser_reflections"].toString();
	if (lref.size()) {
		/* TODO: how to report multiple detections */
		QString ref0 = lref.split(";").first();
		QStringList flds = ref0.split(",");
		addbit(v, NUM_PARAM_RANGE);
		np = response->add_numericparameters();
		np->set_value(flds[0].toInt());

		addbit(v, NUM_PARAM_HEIGHT);
		np = response->add_numericparameters();
		np->set_value(flds[1].toInt());
		/* TODO: we have laser range but no laser height? */
		//addbit(v, NUM_PARAM_LASER_RANGE);
	}

	addbit(v, NUM_PARAM_FOV);
	np = response->add_numericparameters();
	np->set_value(map["fov_pos"].toInt());

	addbit(v, NUM_PARAM_FOCUS);
	np = response->add_numericparameters();
	np->set_value(map["focus_pos_set"].toInt()); /* just in case driver holds last value */

	addbit(v, NUM_PARAM_YAW);
	np = response->add_numericparameters();
	if (pana > 180)
		np->set_value(pana - 360);
	else
		np->set_value(pana);

	addbit(v, NUM_PARAM_PITCH);
	np = response->add_numericparameters();
	np->set_value(tilta);

	addbit(v, NUM_PARAM_HORIZONTAL_RES);
	np = response->add_numericparameters();
	np->set_value(720);

	addbit(v, NUM_PARAM_VERTICAL_RES);
	np = response->add_numericparameters();
	np->set_value(576);

	addbit(v, NUM_PARAM_ZOOM);
	np = response->add_numericparameters();
	np->set_value(map["fov_pos"].toInt());

	/* TODO: What are correct HPF parameters */
	addbit(v, NUM_PARAM_HPF_GAIN);
	np = response->add_numericparameters();
	np->set_value(map["hf_sigma_coeff"].toInt());

	addbit(v, NUM_PARAM_HPF_SPATIAL);
	np = response->add_numericparameters();
	np->set_value(map["hf_filter_std"].toInt());

	response->set_numericparametersvector(v);

	/* enum parameters */

	/* TODO: report correct camera type */
	addbit(v, ENUM_PARAM_CAMERA_TYPE);
	response->add_enumparameters(TV);

	/* TODO: report correct camera op mode */
	addbit(v, ENUM_PARAM_OPERATIONAL_MODE);
	response->add_enumparameters(CONTROL_MODE_WATCH);

	/* TODO: report correct detection creation mode */
	addbit(v, ENUM_PARAM_DETECTION_CREATION_MODE);
	response->add_enumparameters(DETECTION_OPEN_MODE);

	/* TODO: report correct thermal polarity */
	addbit(v, ENUM_PARAM_POLARITY);
	response->add_enumparameters(WHITE_HOT);

	/* TODO: report correct symbology */
	addbit(v, ENUM_PARAM_SEMBOLOGY);
	response->add_enumparameters(SYMBOLOGY_ON);

	response->set_enumparametersvector(v);
}

int64_t KardelenAPIServer::getCapabilities()
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
	addcap(caps, CAPABILITY_RANGE);
	addcap(caps, CAPABILITY_MENU_OVER_VIDEO);
	addcap(caps, CAPABILITY_LAZER_RANGE_FINDER);
	addcap(caps, CAPABILITY_SHOW_HIDE_SEMBOLOGY);
	addcap(caps, CAPABILITY_SHUT_DOWN);
	addcap(caps, CAPABILITY_RESTART);
	addcap(caps, CAPABILITY_AUTO_TRACK_WINDOW);
	addcap(caps, CAPABILITY_AUTO_TRACK_DETECTION);
	addcap(caps, CAPABILITY_NUC);
	addcap(caps, CAPABILITY_FIRE);
	addcap(caps, CAPABILITY_HPF_GAIN);
	addcap(caps, CAPABILITY_HPF_SPATIAL);
	return caps;
}

grpc::Status KardelenAPIServer::GetPosition(grpc::ServerContext *, const google::protobuf::Empty *, kaapi::PosInfo *response)
{
	setPosi(response);
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::MoveAbsolute(grpc::ServerContext *, const kaapi::AbsoluteMoveParameters *request, kaapi::AbsoluteMoveResult *response)
{
	response->set_completionstatus(kaapi::AbsoluteMoveResult_Completion_COMPLETED);
	setPosi(response->mutable_posinfo());
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::MoveRelative(grpc::ServerContext *, const kaapi::RelativeMoveParameters *request, kaapi::RelativeMoveResult *response)
{
	/* speed is no-op for falcon-eye */
	if (request->zoomspeed() > 0)
		ptzp->getHead(0)->startZoomIn(0);
	else if (request->zoomspeed() < 0)
		ptzp->getHead(0)->startZoomOut(0);
	else
		ptzp->getHead(0)->stopZoom();
	ptzp->getHead(1)->panTiltAbs(request->panspeed(), request->tiltspeed());
	kaapi::PosInfo *posi = response->mutable_posinfo();
	setPosi(posi);
	return grpc::Status::OK;
}

grpc::Status KardelenAPIServer::GetCameraStatus(grpc::ServerContext *, const kaapi::GetCameraStatusParameters *, kaapi::CameraStatus *response)
{
	fillCameraStatus(response);
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
		setPosi(msgr.mutable_posinfo());
		fillCameraStatus(msgr.mutable_status());
		stream->Write(msgr);
	}

	return grpc::Status::OK;
}
