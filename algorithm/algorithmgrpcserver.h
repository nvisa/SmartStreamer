#ifndef ALGORITHMGRPCSERVER_H
#define ALGORITHMGRPCSERVER_H

#include <QObject>

#include "proto/AlgorithmWorks.grpc.pb.h"

#include "algorithm/facealgorithmelement.h"
#include "algorithm/trackalgorithmelement.h"
#include "algorithm/motionalgorithmelement.h"
#include "algorithm/panaromaalgorithmelement.h"
#include "algorithm/stabilizationalgorithmelement.h"

class GrpcThread;
class AlgorithmGrpcServer : public aw::AlgorithmWorks::Service
{
public:
	static AlgorithmGrpcServer * instance();

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

	// Tracking Works
	grpc::Status RunTracking(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status StopTracking(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status ReleaseTracking(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status SetTrackingObjInfo(grpc::ServerContext *context, const aw::TrackObjectInfoQ *request, aw::GeneralResponse *response) override;
	grpc::Status SetTrackingDuration(grpc::ServerContext *context, const aw::TrackDurationQ *request, aw::GeneralResponse *response) override;
	grpc::Status SetTrackingScore(grpc::ServerContext *context, const aw::TrackScoreQ *request, aw::GeneralResponse *response) override;
	grpc::Status SetTrackingMultipleMode(grpc::ServerContext *context, const aw::TrackMultipleQ *request, aw::GeneralResponse *response) override;
	grpc::Status GetTrackingDuration(grpc::ServerContext *context, const aw::Empty *request, aw::TrackDurationQ *response) override;
	grpc::Status GetTrackingScore(grpc::ServerContext *context, const aw::Empty *request, aw::TrackScoreQ *response) override;
	grpc::Status GetTrackingMultipleMode(grpc::ServerContext *context, const aw::Empty *request, aw::TrackMultipleQ *response) override;
	grpc::Status GetTrackingState(grpc::ServerContext *context, const aw::Empty *request, aw::AlgoResponse *response) override;

	// Face Works
	grpc::Status RunFace(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status StopFace(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status ReleaseFace(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status SetFaceCamID(grpc::ServerContext *context, const aw::FaceCamIDQ *request, aw::GeneralResponse *response) override;
	grpc::Status SetFaceFrameRate(grpc::ServerContext *context, const aw::FaceFrameRateQ *request, aw::GeneralResponse *response) override;
	grpc::Status GetFaceCamID(grpc::ServerContext *context, const aw::Empty *request, aw::FaceCamIDQ *response) override;
	grpc::Status GetFaceFrameRate(grpc::ServerContext *context, const aw::Empty *request, aw::FaceFrameRateQ *response) override;
	grpc::Status GetFaceState(grpc::ServerContext *context, const aw::Empty *request, aw::AlgoResponse *response) override;

	// Panaroma Works
	grpc::Status RunPanaroma(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status StopPanaroma(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status ReleasePanaroma(grpc::ServerContext *context, const aw::Empty *request, aw::RunStopResponse *response) override;
	grpc::Status GetPanaromaState(grpc::ServerContext *context, const aw::Empty *request, aw::AlgoResponse *response) override;

	void setFaceAlgorithmElement(FaceAlgorithmElement *el) {faceEl = el;}
	void setTrackAlgorithmElement(TrackAlgorithmElement *el) {trackEl = el;}
	void setMotionAlgorithmElement(MotionAlgorithmElement *el) {motionEl = el;}
	void setPanaromaAlgorithmElement(PanaromaAlgorithmElement *el) {panaromaEl = el;}
	void setStabilizationAlgorithmElement(StabilizationAlgorithmElement *el) {stabilizationEl = el;}
signals:

public slots:
protected:
	explicit AlgorithmGrpcServer(QObject *parent = 0);

	GrpcThread *grpcServ;
	FaceAlgorithmElement *faceEl;
	TrackAlgorithmElement *trackEl;
	MotionAlgorithmElement *motionEl;
	PanaromaAlgorithmElement *panaromaEl;
	StabilizationAlgorithmElement *stabilizationEl;
};

#endif // ALGORITHMGRPCSERVER_H
