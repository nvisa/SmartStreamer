#ifndef ORIONCOMMUNICATIONSERVER_H
#define ORIONCOMMUNICATIONSERVER_H

#include "proto/OrionCommunication.pb.h"
#include "proto/OrionCommunication.grpc.pb.h"

#include "snapshotelement.h"
#include "algorithm/motionalgorithmelement.h"
#include "algorithm/panaromaalgorithmelement.h"

class OrionCommunicationServer: public OrionCommunication::OrionCommunicationService::Service
{
public:
	static OrionCommunicationServer * instance();
	void setSnapshotElement(SnapshotElement *el) {snapshotEl = el;}
	void setMotionAlgorithmElement(MotionAlgorithmElement *el) {motionEl = el;}
	void setPanaromaAlgorithmElement(PanaromaAlgorithmElement *el) {panaromaEl = el;}

	grpc::Status RunPanaroma(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response);
	grpc::Status StopPanaroma(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response);
	grpc::Status RunMotion(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response);
	grpc::Status StopMotion(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response);
	grpc::Status SetMotionDetectionParameters(grpc::ServerContext *context, const OrionCommunication::TRoi *request, OrionCommunication::AppCommandResult *response);
	grpc::Status GetMotionDetectionParameters(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::TRoi *response);
	grpc::Status GetSensivityParameter(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::SetSensivity *response);
	grpc::Status SetSensivityParameter(grpc::ServerContext *context, const OrionCommunication::SetSensivity *request, OrionCommunication::AppCommandResult *response);
	grpc::Status GotoPanaromaPixel(grpc::ServerContext *context, const OrionCommunication::TPoint *request, OrionCommunication::AppCommandResult *response);
	grpc::Status GetPanaromaFrames(grpc::ServerContext *context, const OrionCommunication::FrameModeQ *request, ::grpc::ServerWriter<OrionCommunication::PanoramaFrame> *writer);
	grpc::Status GetMainScreenShot(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::ScreenFrame *response);
	grpc::Status SetVideoOverlay(grpc::ServerContext *context, const OrionCommunication::OverlayQ *request, OrionCommunication::AppCommandResult *response);
protected:
	OrionCommunicationServer();
private:
	MotionAlgorithmElement *motionEl;
	PanaromaAlgorithmElement *panaromaEl;
	SnapshotElement *snapshotEl;

};

#endif // ORIONCOMMUNICATIONSERVER_H
