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
	GrpcThreadAlg *grpcServ = new GrpcThreadAlg(50059, this);
	grpcServ->start();
}

grpc::Status AlgorithmGrpcServer::RunMotion(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (motionEl)
		motionEl->setState(BaseAlgorithmElement::INIT);
	response->set_response(aw::RunStopResponse::SUCCESS);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::StopMotion(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (motionEl)
		motionEl->setState(BaseAlgorithmElement::STOPALGO);
	response->set_response(aw::RunStopResponse::SUCCESS);
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::ReleaseMotion(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response)
{
	if (!motionEl) {
		response->set_response(aw::RunStopResponse::FAIL);
		return grpc::Status::OK;
	}
	motionEl->setState(BaseAlgorithmElement::RELEASE);
	response->set_response(aw::RunStopResponse::SUCCESS);
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

