#ifndef ALGORITHMGRPCSERVER_H
#define ALGORITHMGRPCSERVER_H

#include <QObject>

#include "snapshotelement.h"
#include "proto/AlgorithmCommunication.grpc.pb.h"

#include "algorithm/facealgorithmelement.h"
#include "algorithm/trackalgorithmelement.h"
#include "algorithm/motionalgorithmelement.h"
#include "algorithm/panaromaalgorithmelement.h"
#include "algorithm/stabilizationalgorithmelement.h"

class AlgorithmGrpcServer : public AlgorithmCommunication::AlgorithmService::Service
{
public:
	static AlgorithmGrpcServer * instance();

	void setFaceAlgorithmElement(FaceAlgorithmElement *el) {faceEl = el;}
	void setTrackAlgorithmElement(TrackAlgorithmElement *el) {trackEl = el;}
	void setMotionAlgorithmElement(MotionAlgorithmElement *el) {motionEl = el;}
	void setPanaromaAlgorithmElement(PanaromaAlgorithmElement *el) {panaromaEl = el;}
	void setStabilizationAlgorithmElement(StabilizationAlgorithmElement *el) {stabilizationEl = el;}
	void setSnapshotElement(SnapshotElement *el) {snapshotEl = el;}
	grpc::Status RunAlgorithm(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response);
	grpc::Status StopAlgorithm(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response);

	grpc::Status SetAlgorithmParameters(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response);
	grpc::Status GetAlgorithmParameters(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfParameterRequests *response);

	grpc::Status GetScreenShot(grpc::ServerContext *context, const AlgorithmCommunication::ScreenshotInfo *request, AlgorithmCommunication::ScreenFrame *response);
	grpc::Status GetScreenShotStream(grpc::ServerContext *context, const AlgorithmCommunication::ScreenshotInfo *request, ::grpc::ServerWriter<AlgorithmCommunication::ScreenFrame> *writer);

protected:
	explicit AlgorithmGrpcServer(QObject *parent = 0);
	int setMotionParameters(AlgorithmCommunication::MotionParameters p);
	int getMotionParameters(AlgorithmCommunication::MotionParameters p);
private:
	FaceAlgorithmElement *faceEl;
	TrackAlgorithmElement *trackEl;
	MotionAlgorithmElement *motionEl;
	PanaromaAlgorithmElement *panaromaEl;
	StabilizationAlgorithmElement *stabilizationEl;
	SnapshotElement *snapshotEl;
};

#endif // ALGORITHMGRPCSERVER_H
