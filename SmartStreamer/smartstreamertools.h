#ifndef SMARTSTREAMERTOOLS_H
#define SMARTSTREAMERTOOLS_H

#include <QString>

#include <grpc++/client_context.h>

#include "proto/AlgorithmCommunication.grpc.pb.h"
#include "proto/OrionCommunication.grpc.pb.h"
#include "proto/v2/AlgorithmCommunicationV2.grpc.pb.h"

class SmartStreamerTools
{
public:
	SmartStreamerTools();
	int toolsMain(int argc, char *argv[]);
	int testGrpcV2(const QString &action, const QString &target);
	int testOrionGrpc(const QString &action, const QString &target);
	int testGrpc(const QString &action, const QString &target);
	int kardelenAPIClient(const QString &action, QStringList args, const QString &target);
protected:
	grpc::ClientContext ctx;
	grpc::Status status;
	QString target;
};

#endif // SMARTSTREAMERTOOLS_H
