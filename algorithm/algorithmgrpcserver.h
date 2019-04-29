#ifndef ALGORITHMGRPCSERVER_H
#define ALGORITHMGRPCSERVER_H

#include <QObject>

#include "proto/AlgorithmWorks.grpc.pb.h"

#include "algorithm/motionalgorithmelement.h"

class GrpcThread;
class AlgorithmGrpcServer : public aw::AlgorithmWorks::Service
{
public:
	explicit AlgorithmGrpcServer(QObject *parent = 0);
	grpc::Status RunMotion(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status StopMotion(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status ReleaseMotion(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status GetMotionState(grpc::ServerContext *context, const aw::Empty *request, aw::AlgoResponse *response) override;
	grpc::Status SetMotionRoi(grpc::ServerContext *context, const aw::RoiQ *request, aw::GeneralResponse *response) override;
	grpc::Status SetSensitivity(grpc::ServerContext *context, const aw::Sensitivity *request, aw::GeneralResponse *response) override;
	void setMotionAlgorithmElement(MotionAlgorithmElement *el) {motionEl = el;}
signals:

public slots:
protected:
	GrpcThread *grpcServ;
	MotionAlgorithmElement *motionEl;
};

#endif // ALGORITHMGRPCSERVER_H
