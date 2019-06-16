#include "algorithmgrpcserver.h"
#include "diagnosticserviceimpl.h"

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
#include "proto/AlgorithmCommunication.grpc.pb.h"
#include "proto/AlgorithmCommunication.pb.h"

#include "ecl/ptzp/ptzphead.h"
#include "ecl/ptzp/ptzpdriver.h"
#include "applicationinfo.h"

#include <QFile>
#include <QImage>
#include <QBuffer>
#include <unistd.h>

class GrpcThreadAlg1 : public QThread
{
public:
	GrpcThreadAlg1(quint16 port, AlgorithmGrpcServer *s)
	{
		servicePort = port;
		algorithm = s;
	}
	void run()
	{
		string ep(qPrintable(QString("0.0.0.0:%1").arg(servicePort)));
		ServerBuilder builder;
		builder.AddListeningPort(ep, grpc::InsecureServerCredentials());
		builder.RegisterService(algorithm);
		DiagnosticServiceImpl *diag = new DiagnosticServiceImpl(NULL);
		builder.RegisterService(diag);
		std::unique_ptr<Server> server(builder.BuildAndStart());
		server->Wait();
	}

protected:
	int servicePort;
	AlgorithmGrpcServer *algorithm;
};


static AlgorithmGrpcServer *inst = NULL;


AlgorithmGrpcServer::AlgorithmGrpcServer(QObject *parent)
	: AlgorithmCommunication::AlgorithmService::Service()
{
	faceEl = NULL;
	trackEl = NULL;
	motionEl = NULL;
	panaromaEl = NULL;
	stabilizationEl = NULL;
	snapshotEl = NULL;

	GrpcThreadAlg1 *grpcServ = new GrpcThreadAlg1(50059, this);
	grpcServ->start();
}

AlgorithmGrpcServer *AlgorithmGrpcServer::instance()
{
	if (!inst)
		inst = new AlgorithmGrpcServer;
	return inst;
}

grpc::Status AlgorithmGrpcServer::RunAlgorithm(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response)
{
	int chn = request->channel();
	AlgorithmCommunication::TrackParameters trackpar = request->trackparam();
	AlgorithmCommunication::MotionParameters motionpar = request->motionparam();
	AlgorithmCommunication::PanaromaParameters panaromapar = request->panaromaparam();
	AlgorithmCommunication::StabilizationParameters stabilizationpar = request->stabilizationparam();
	AlgorithmCommunication::FaceDetectionParameters facedetectionpar = request->facedetectionparam();

	switch (request->algorithmtype()) {
	case AlgorithmCommunication::RequestForAlgorithm::MOTION:
		if (!setMotionParameters(motionpar))
			motionEl->setState(BaseAlgorithmElement::INIT);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::STABILIZATION:
		break;
	case AlgorithmCommunication::RequestForAlgorithm::TRACKING:
		break;
	case AlgorithmCommunication::RequestForAlgorithm::PANAROMA:
		panaromaEl->setState(BaseAlgorithmElement::INIT);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::FACE_DETECTION:
		break;
	default:
		break;
	}
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::StopAlgorithm(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response)
{
	int chn = request->channel();
	AlgorithmCommunication::TrackParameters trackpar = request->trackparam();
	AlgorithmCommunication::MotionParameters motionpar = request->motionparam();
	AlgorithmCommunication::PanaromaParameters panaromapar = request->panaromaparam();
	AlgorithmCommunication::StabilizationParameters stabilizationpar = request->stabilizationparam();
	AlgorithmCommunication::FaceDetectionParameters facedetectionpar = request->facedetectionparam();

	switch (request->algorithmtype()) {
	case AlgorithmCommunication::RequestForAlgorithm::MOTION:
		motionEl->setState(BaseAlgorithmElement::STOPALGO);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::STABILIZATION:
		break;
	case AlgorithmCommunication::RequestForAlgorithm::TRACKING:
		break;
	case AlgorithmCommunication::RequestForAlgorithm::PANAROMA:
		panaromaEl->setState(BaseAlgorithmElement::STOPALGO);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::FACE_DETECTION:
		break;
	default:
		break;
	}
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::SetAlgorithmParameters(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response)
{
	AlgorithmCommunication::TrackParameters trackpar = request->trackparam();
	AlgorithmCommunication::MotionParameters motionpar = request->motionparam();
	AlgorithmCommunication::PanaromaParameters panaromapar = request->panaromaparam();
	AlgorithmCommunication::StabilizationParameters stabilizationpar = request->stabilizationparam();
	AlgorithmCommunication::FaceDetectionParameters facedetectionpar = request->facedetectionparam();

	switch (request->algorithmtype()) {
	case AlgorithmCommunication::RequestForAlgorithm::MOTION:
		setMotionParameters(motionpar);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::STABILIZATION:
		break;
	case AlgorithmCommunication::RequestForAlgorithm::TRACKING:
		break;
	case AlgorithmCommunication::RequestForAlgorithm::PANAROMA:
		break;
	case AlgorithmCommunication::RequestForAlgorithm::FACE_DETECTION:
		break;
	default:
		break;
	}
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::GetAlgorithmParameters(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfParameterRequests *response)
{
	AlgorithmCommunication::MotionParameters *p;
	switch (request->algorithmtype()) {
	case AlgorithmCommunication::RequestForAlgorithm::MOTION:
		p = motionEl->loadROI();
		p->set_sensitivity(motionEl->getSensitivity());
		response->set_allocated_motionparam(p);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::STABILIZATION:
		break;
	case AlgorithmCommunication::RequestForAlgorithm::TRACKING:
		break;
	case AlgorithmCommunication::RequestForAlgorithm::PANAROMA:
		break;
	case AlgorithmCommunication::RequestForAlgorithm::FACE_DETECTION:
		break;
	default:
		break;
	}
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::GetScreenShot(grpc::ServerContext *context, const AlgorithmCommunication::ScreenshotInfo *request, AlgorithmCommunication::ScreenFrame *response)
{
	if (!snapshotEl)
		return grpc::Status::CANCELLED;
	QByteArray ba = snapshotEl->take(1);
	response->set_frame(ba, ba.size());
	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServer::GetScreenShotStream(grpc::ServerContext *context, const AlgorithmCommunication::ScreenshotInfo *request, ::grpc::ServerWriter<AlgorithmCommunication::ScreenFrame> *writer)
{
	if (!snapshotEl)
		return grpc::Status::CANCELLED;
	while(1) {
		QByteArray ba = snapshotEl->take(1);
		AlgorithmCommunication::ScreenFrame frame;
		frame.set_frame(ba, ba.size());
		writer->Write(frame);
		usleep(10000);
	}
	return grpc::Status::OK;
}

int AlgorithmGrpcServer::setMotionParameters(AlgorithmCommunication::MotionParameters p)
{
	int ret = 0;
	switch (p.settingchoice()) {
	case AlgorithmCommunication::MotionParameters::SENSITIVITY:
		motionEl->setSensitivity(p.sensitivity());
		break;
	case AlgorithmCommunication::MotionParameters::ROI:
		ret = motionEl->saveROI(p.roilist());
		break;
	case AlgorithmCommunication::MotionParameters::BOTH:
		motionEl->setSensitivity(p.sensitivity());
		ret = motionEl->saveROI(p.roilist());
		break;
	default:
		break;
	}
	return ret;
}

