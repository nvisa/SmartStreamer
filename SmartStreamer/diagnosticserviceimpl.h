#ifndef DIAGNOSTICSERVICEIMPL_H
#define DIAGNOSTICSERVICEIMPL_H

#include "lmm/players/basestreamer.h"

#include <proto/AlgorithmCommunication.grpc.pb.h>

class DiagnosticServiceImpl : public AlgorithmCommunication::DiagnosticService::Service
{
public:
	DiagnosticServiceImpl(BaseStreamer *streamer);

	grpc::Status GetDiagnosticStatus(grpc::ServerContext *context, const AlgorithmCommunication::DiagnosticQ *request, AlgorithmCommunication::DiagnosticInfo *response) override;
	grpc::Status GetDiagnosticStatusStream(grpc::ServerContext *context, const AlgorithmCommunication::DiagnosticQ *request, ::grpc::ServerWriter<AlgorithmCommunication::DiagnosticInfo> *writer) override;
private:
	BaseStreamer *serviceStreamer;
};

#endif // DIAGNOSTICSERVICEIMPL_H
