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
	motionEl = NULL;
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

