#include "peercheck.h"

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/channel.h>
#include <grpc++/create_channel.h>
#include <grpc++/client_context.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/credentials.h>
#include <grpc++/security/server_credentials.h>

#include "proto/OrionCommunication.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::Status;

using namespace std;

PeerCheck::PeerCheck()
{

}

bool PeerCheck::isAlive(QString ip)
{
	QString ep = QString("%1:50059").arg(ip);
	std::shared_ptr<grpc::Channel> chn = grpc::CreateChannel(ep.toStdString(), grpc::InsecureChannelCredentials());
	std::shared_ptr<OrionCommunication::OrionCommunicationService::Stub> stub = OrionCommunication::OrionCommunicationService::NewStub(chn);
	grpc::ClientContext ctx;
	OrionCommunication::DevicedBasedInfo req;
	req.set_device(OrionCommunication::DevicedBasedInfo::DeviceType::DevicedBasedInfo_DeviceType_TV);
	OrionCommunication::ReturnUptimeValue resp;
	if (stub->GetUptime(&ctx, req, &resp).error_code() == grpc::StatusCode::OK)
		return true;
	return false;
}

void PeerCheck::leaveEvpuAlone(QString ip)
{
	QString ep = QString("%1:50059").arg(ip);
	std::shared_ptr<grpc::Channel> chn = grpc::CreateChannel(ep.toStdString(), grpc::InsecureChannelCredentials());
	std::shared_ptr<OrionCommunication::OrionCommunicationService::Stub> stub = OrionCommunication::OrionCommunicationService::NewStub(chn);
	grpc::ClientContext ctx;
	OrionCommunication::DummyInfo req;
	OrionCommunication::AppCommandResult resp;
	stub->RunMotion(&ctx, req, &resp);
}

