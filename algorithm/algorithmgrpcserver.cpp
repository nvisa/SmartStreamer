#include "algorithmgrpcserver.h"

#include <lmm/debug.h>

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
using namespace std;

#include <QThread>
#include "proto/AlgorithmWorks.grpc.pb.h"
#include "proto/AlgorithmWorks.pb.h"
#include "algorithm/basealgorithmcommon.h"

class GrpcThreadAlg : public QThread
{
public:
	GrpcThreadAlg (quint16 port, AlgorithmGrpcServer *s)
	{
		servicePort = port;
		algorithm = s;
	}
	void run()
	{
		string ep(qPrintable(QString("0.0.0.0:%1").arg(servicePort)));
		qDebug() << "--------------------------------------------------" << QString::fromStdString(ep);
		ServerBuilder builder;
		builder.AddListeningPort(ep, grpc::InsecureServerCredentials());
		builder.RegisterService(algorithm);
		std::unique_ptr<Server> server(builder.BuildAndStart());
		server->Wait();
	}

protected:
	int servicePort;
	AlgorithmGrpcServer *algorithm;
};


AlgorithmGrpcServer::AlgorithmGrpcServer(QObject *parent)
	: aw::AlgorithmWorks::Service()
{
	faceEl = NULL;
	trackEl = NULL;
	motionEl = NULL;
	panaromaEl = NULL;
	stabilizationEl = NULL;

	GrpcThreadAlg *grpcServ = new GrpcThreadAlg(50059, this);
	grpcServ->start();
}

grpc::Status AlgorithmGrpcServer::RunMotion(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (motionEl) {
		motionEl->setState(BaseAlgorithmElement::INIT);
		response->set_response(aw::RunStopResponse::SUCCESS);
	} else
		response->set_response(aw::RunStopResponse::FAIL);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::StopMotion(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (motionEl) {
		if (motionEl->getState() == BaseAlgorithmElement::STOPALGO)
			motionEl->setState(BaseAlgorithmElement::PROCESS);
		else
			motionEl->setState(BaseAlgorithmElement::STOPALGO);
		response->set_response(aw::RunStopResponse::SUCCESS);
	} else
		response->set_response(aw::RunStopResponse::FAIL);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::ReleaseMotion(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (motionEl) {
		motionEl->setState(BaseAlgorithmElement::RELEASE);
		response->set_response(aw::RunStopResponse::SUCCESS);
	} else
		response->set_response(aw::RunStopResponse::FAIL);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::GetMotionState(grpc::ServerContext *context, const aw::Empty *request, aw::AlgoResponse *response)
{
	if (!motionEl)
		return grpc::Status::CANCELLED;
	aw::AlgoResponse::AlgoState state;
	state = static_cast<aw::AlgoResponse::AlgoState>(motionEl->getState());
	response->set_state(state);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::SetMotionRoi(grpc::ServerContext *context, const aw::RoiQ *request, aw::GeneralResponse *response)
{
	BaseAlgorithmCommon *baseAlgo = BaseAlgorithmCommon::instance();
	int err = baseAlgo->saveRoiPoints(*request);
	if (err) {
		response->set_response(aw::GeneralResponse::FAIL);
		response->set_error_code(err);
		return grpc::Status::CANCELLED;
	}
	response->set_response(aw::GeneralResponse::SUCCESS);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::RunStabilization(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (stabilizationEl) {
		stabilizationEl->setState(BaseAlgorithmElement::INIT);
		response->set_response(aw::RunStopResponse::SUCCESS);
	} else
		response->set_response(aw::RunStopResponse::FAIL);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::StopStabilization(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (stabilizationEl) {
		if (stabilizationEl->getState() == BaseAlgorithmElement::STOPALGO)
			stabilizationEl->setState(BaseAlgorithmElement::PROCESS);
		else stabilizationEl->setState(BaseAlgorithmElement::STOPALGO);
		response->set_response(aw::RunStopResponse::SUCCESS);
	} else
		response->set_response(aw::RunStopResponse::FAIL);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::ReleaseStabilization(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (stabilizationEl) {
		stabilizationEl->setState(BaseAlgorithmElement::RELEASE);
		response->set_response(aw::RunStopResponse::SUCCESS);
	}
	else
		response->set_response(aw::RunStopResponse::FAIL);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::GetStabilizationState(grpc::ServerContext *context, const aw::Empty *request, aw::AlgoResponse *response)
{
	if (!stabilizationEl)
		return grpc::Status::CANCELLED;
	aw::AlgoResponse::AlgoState state;
	state = static_cast<aw::AlgoResponse::AlgoState>(stabilizationEl->getState());
	response->set_state(state);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::SetSensitivity(grpc::ServerContext *context, const aw::Sensitivity *request, aw::GeneralResponse *response)
{
	BaseAlgorithmCommon *baseAlgo = BaseAlgorithmCommon::instance();
	int err = -1;
	if (request->type() == aw::Sensitivity::MOTION)
		err = baseAlgo->setSensitivity("motion_detection", request->sensitivity());
	else if (request->type() == aw::Sensitivity::STABILIZATION)
		err = baseAlgo->setSensitivity("stabilization", request->sensitivity());
	if (err) {
		response->set_response(aw::GeneralResponse::FAIL);
		response->set_error_code(err);
		return grpc::Status::CANCELLED;
	}
	response->set_response(aw::GeneralResponse::SUCCESS);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::GetSensitivity(grpc::ServerContext *context, const aw::Sensitivity *request, aw::Sensitivity *response)
{
	BaseAlgorithmCommon *baseAlgo = BaseAlgorithmCommon::instance();
	int ret = 0;
	if (request->type() == aw::Sensitivity::MOTION)
		ret = baseAlgo->getSensitivity("motion_detection");
	else if (request->type() == aw::Sensitivity::STABILIZATION)
		ret = baseAlgo->getSensitivity("stabilization");
	if (ret < 0)
		return grpc::Status::CANCELLED;
	response->set_sensitivity(ret);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::RunTracking(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (trackEl) {
		trackEl->setState(BaseAlgorithmElement::INIT);
		response->set_response(aw::RunStopResponse::SUCCESS);
	} else
		response->set_response(aw::RunStopResponse::FAIL);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::StopTracking(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (trackEl) {
		if (trackEl->getState() == BaseAlgorithmElement::STOPALGO)
			trackEl->setState(BaseAlgorithmElement::PROCESS);
		else
			trackEl->setState(BaseAlgorithmElement::STOPALGO);
		response->set_response(aw::RunStopResponse::SUCCESS);
	} else
		response->set_response(aw::RunStopResponse::FAIL);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::ReleaseTracking(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (trackEl) {
		trackEl->setState(BaseAlgorithmElement::RELEASE);
		response->set_response(aw::RunStopResponse::SUCCESS);
	} else
		response->set_response(aw::RunStopResponse::FAIL);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::SetTrackingObjInfo(grpc::ServerContext *context, const aw::TrackObjectInfoQ *request, aw::GeneralResponse *response)
{
	if (!trackEl) {
		response->set_response(aw::GeneralResponse::FAIL);
		response->set_error_code(19);
		return grpc::Status::OK;
	}
	trackEl->setTrackObjInfo(request->point_x(), request->point_y(), request->width(), request->height());
	response->set_response(aw::GeneralResponse::SUCCESS);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::SetTrackingDuration(grpc::ServerContext *context, const aw::TrackDurationQ *request, aw::GeneralResponse *response)
{
	int ret = 0;
	BaseAlgorithmCommon *baseAlgo = BaseAlgorithmCommon::instance();
	ret = baseAlgo->setTrackingDuration(request->duration());
	if (ret)
		response->set_response(aw::GeneralResponse::FAIL);
	else
		response->set_response(aw::GeneralResponse::SUCCESS);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::SetTrackingScore(grpc::ServerContext *context, const aw::TrackScoreQ *request, aw::GeneralResponse *response)
{
	int ret = 0;
	BaseAlgorithmCommon *baseAlgo = BaseAlgorithmCommon::instance();
	ret = baseAlgo->setTrackingScore(request->score());
	if (ret)
		response->set_response(aw::GeneralResponse::FAIL);
	else
		response->set_response(aw::GeneralResponse::SUCCESS);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::SetTrackingMultipleMode(grpc::ServerContext *context, const aw::TrackMultipleQ *request, aw::GeneralResponse *response)
{
	int ret = 0;
	BaseAlgorithmCommon *baseAlgo = BaseAlgorithmCommon::instance();
	ret = baseAlgo->setTrackingMultiple(request->multiple());
	if (ret)
		response->set_response(aw::GeneralResponse::FAIL);
	else
		response->set_response(aw::GeneralResponse::SUCCESS);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::GetTrackingDuration(grpc::ServerContext *context, const aw::Empty *request, aw::TrackDurationQ *response)
{
	int ret = -1;
	BaseAlgorithmCommon *baseAlgo = BaseAlgorithmCommon::instance();
	ret = baseAlgo->getTrackingDuration();
	response->set_duration(ret);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::GetTrackingScore(grpc::ServerContext *context, const aw::Empty *request, aw::TrackScoreQ *response)
{
	int ret = -1;
	BaseAlgorithmCommon *baseAlgo = BaseAlgorithmCommon::instance();
	ret = baseAlgo->getTrackingScore();
	response->set_score(ret);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::GetTrackingMultipleMode(grpc::ServerContext *context, const aw::Empty *request, aw::TrackMultipleQ *response)
{
	int ret = -1;
	BaseAlgorithmCommon *baseAlgo = BaseAlgorithmCommon::instance();
	ret = baseAlgo->getTrackingMultiple();
	response->set_multiple(ret);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::RunFace(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (faceEl) {
		faceEl->setState(BaseAlgorithmElement::INIT);
		response->set_response(aw::RunStopResponse::SUCCESS);
	} else
		response->set_response(aw::RunStopResponse::FAIL);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::StopFace(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (faceEl) {
		if (faceEl->getState() == BaseAlgorithmElement::STOPALGO)
			faceEl->setState(BaseAlgorithmElement::PROCESS);
		else
			faceEl->setState(BaseAlgorithmElement::STOPALGO);
		response->set_response(aw::RunStopResponse::SUCCESS);
	} else
		response->set_response(aw::RunStopResponse::FAIL);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::ReleaseFace(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (faceEl) {
		faceEl->setState(BaseAlgorithmElement::RELEASE);
		response->set_response(aw::RunStopResponse::SUCCESS);
	} else
		response->set_response(aw::RunStopResponse::FAIL);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::SetFaceCamID(grpc::ServerContext *context, const aw::FaceCamIDQ *request, aw::GeneralResponse *response)
{
	int ret = 0;
	BaseAlgorithmCommon *baseAlgo = BaseAlgorithmCommon::instance();
	ret = baseAlgo->setFaceCamID(request->camid());
	if (ret)
		response->set_response(aw::GeneralResponse::FAIL);
	else
		response->set_response(aw::GeneralResponse::SUCCESS);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::SetFaceFrameRate(grpc::ServerContext *context, const aw::FaceFrameRateQ *request, aw::GeneralResponse *response)
{
	int ret = 0;
	BaseAlgorithmCommon *baseAlgo = BaseAlgorithmCommon::instance();
	ret = baseAlgo->setFaceFrameRate(request->frame_rate());
	if (ret)
		response->set_response(aw::GeneralResponse::FAIL);
	else
		response->set_response(aw::GeneralResponse::SUCCESS);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::GetFaceCamID(grpc::ServerContext *context, const aw::Empty *request, aw::FaceCamIDQ *response)
{
	int ret = -1;
	BaseAlgorithmCommon *baseAlgo = BaseAlgorithmCommon::instance();
	ret = baseAlgo->getFaceCamID();
	response->set_camid(ret);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::GetFaceFrameRate(grpc::ServerContext *context, const aw::Empty *request, aw::FaceFrameRateQ *response)
{
	int ret = -1;
	BaseAlgorithmCommon *baseAlgo = BaseAlgorithmCommon::instance();
	ret = baseAlgo->getFaceFrameRate();
	response->set_frame_rate(ret);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::RunPanaroma(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (panaromaEl) {
		panaromaEl->setState(BaseAlgorithmElement::INIT);
		response->set_response(aw::RunStopResponse::SUCCESS);
	} else
		response->set_response(aw::RunStopResponse::FAIL);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::StopPanaroma(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (panaromaEl) {
		if (panaromaEl->getState() == BaseAlgorithmElement::STOPALGO)
			panaromaEl->setState(BaseAlgorithmElement::PROCESS);
		else
			panaromaEl->setState(BaseAlgorithmElement::STOPALGO);
		response->set_response(aw::RunStopResponse::SUCCESS);
	} else
		response->set_response(aw::RunStopResponse::FAIL);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::ReleasePanaroma(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (panaromaEl) {
		panaromaEl->setState(BaseAlgorithmElement::RELEASE);
		response->set_response(aw::RunStopResponse::SUCCESS);
	} else
		response->set_response(aw::RunStopResponse::FAIL);
	return grpc::Status::OK;
}

