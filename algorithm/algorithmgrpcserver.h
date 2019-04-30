#ifndef ALGORITHMGRPCSERVER_H
#define ALGORITHMGRPCSERVER_H

#include <QObject>

#include "proto/AlgorithmWorks.grpc.pb.h"

#include "algorithm/motionalgorithmelement.h"
#include "algorithm/stabilizationalgorithmelement.h"

class GrpcThread;
class AlgorithmGrpcServer : public aw::AlgorithmWorks::Service
{
public:
	explicit AlgorithmGrpcServer(QObject *parent = 0);
	// MotionWorks
	grpc::Status RunMotion(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status StopMotion(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status ReleaseMotion(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status GetMotionState(grpc::ServerContext *context, const aw::Empty *request, aw::AlgoResponse *response) override;
	grpc::Status SetMotionRoi(grpc::ServerContext *context, const aw::RoiQ *request, aw::GeneralResponse *response) override;
	// StabilizationWorks
	grpc::Status RunStabilization(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status StopStabilization(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status ReleaseStabilization(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status GetStabilizationState(grpc::ServerContext *context, const aw::Empty *request, aw::AlgoResponse *response) override;
	// GeneralWorks
	grpc::Status SetSensitivity(grpc::ServerContext *context, const aw::Sensitivity *request, aw::GeneralResponse *response) override;
	grpc::Status GetSensitivity(grpc::ServerContext *context, const aw::Sensitivity *request, aw::Sensitivity *response) override;

	void setMotionAlgorithmElement(MotionAlgorithmElement *el) {motionEl = el;}
	void setStabilizationAlgorithmElement(StabilizationAlgorithmElement *el) {stabilizationEl = el;}
signals:

public slots:
protected:
	GrpcThread *grpcServ;
	MotionAlgorithmElement *motionEl;
	StabilizationAlgorithmElement *stabilizationEl;
};

#endif // ALGORITHMGRPCSERVER_H
