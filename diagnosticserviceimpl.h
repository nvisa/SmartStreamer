#ifndef DIAGNOSTICSERVICEIMPL_H
#define DIAGNOSTICSERVICEIMPL_H

#include "lmm/players/basestreamer.h"

#include <proto/AlgorithmWorks.grpc.pb.h>

class DiagnosticServiceImpl : public aw::DiagnosticService::Service
{
public:
	DiagnosticServiceImpl(BaseStreamer *streamer);

	grpc::Status GetDiagnosticStatus(grpc::ServerContext *context, const aw::DiagnosticQ *request, aw::DiagnosticInfo *response) override;
	grpc::Status GetDiagnosticStatusStream(grpc::ServerContext *context, const aw::DiagnosticQ *request, ::grpc::ServerWriter<aw::DiagnosticInfo> *writer) override;
private:
	BaseStreamer *serviceStreamer;
};

#endif // DIAGNOSTICSERVICEIMPL_H
