#include "orioncommunicationserver.h"
#include <QThread>

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
#include "applicationinfo.h"

#include <lmm/debug.h>
#include <ecl/ptzp/ptzpdriver.h>

#include <QFile>
#include <unistd.h>

class GrpcThreadAlg2 : public QThread
{
public:
	GrpcThreadAlg2(quint16 port, OrionCommunicationServer *s)
	{
		servicePort = port;
		orion = s;
	}
	void run()
	{
		string ep(qPrintable(QString("0.0.0.0:%1").arg(servicePort)));
		ServerBuilder builder;
		builder.AddListeningPort(ep, grpc::InsecureServerCredentials());
		builder.RegisterService(orion);
		std::unique_ptr<Server> server(builder.BuildAndStart());
		server->Wait();
	}

protected:
	int servicePort;
	OrionCommunicationServer *orion;
};

static OrionCommunicationServer *inst = NULL;

OrionCommunicationServer *OrionCommunicationServer::instance()
{
	if (!inst)
		inst = new OrionCommunicationServer;
	return inst;
}

grpc::Status OrionCommunicationServer::RunPanaroma(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
	if (!panaromaEl)
		return grpc::Status::CANCELLED;
	panaromaEl->setState(BaseAlgorithmElement::INIT);
	return grpc::Status::OK;
}

grpc::Status OrionCommunicationServer::StopPanaroma(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
	if (!panaromaEl)
		return grpc::Status::CANCELLED;
	panaromaEl->setState(BaseAlgorithmElement::STOPALGO);
	return grpc::Status::OK;
}

grpc::Status OrionCommunicationServer::RunMotion(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
	if (!motionEl)
		return grpc::Status::CANCELLED;
	motionEl->setState(BaseAlgorithmElement::INIT);
	return grpc::Status::OK;
}

grpc::Status OrionCommunicationServer::StopMotion(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
	if (!motionEl)
		return grpc::Status::CANCELLED;
	if (motionEl->getState() == BaseAlgorithmElement::PROCESS)
		motionEl->setState(BaseAlgorithmElement::STOPALGO);
	return grpc::Status::OK;
}

grpc::Status OrionCommunicationServer::SetMotionDetectionParameters(grpc::ServerContext *context, const OrionCommunication::TRoi *request, OrionCommunication::AppCommandResult *response)
{
	if (!motionEl)
		return grpc::Status::CANCELLED;
	motionEl->saveROI(request);
	return grpc::Status::OK;
}

grpc::Status OrionCommunicationServer::GetMotionDetectionParameters(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::TRoi *response)
{
	if (!motionEl)
		return grpc::Status::CANCELLED;
	*response = *motionEl->loadROIOrion();
	return grpc::Status::OK;
}

grpc::Status OrionCommunicationServer::GetSensivityParameter(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::SetSensivity *response)
{
	if (!motionEl)
		return grpc::Status::CANCELLED;
	response->set_sensivity(motionEl->getSensitivity());
	return grpc::Status::OK;
}

grpc::Status OrionCommunicationServer::SetSensivityParameter(grpc::ServerContext *context, const OrionCommunication::SetSensivity *request, OrionCommunication::AppCommandResult *response)
{
	if (!motionEl)
		return grpc::Status::CANCELLED;
	motionEl->setSensitivity(request->sensivity());
	response->set_err(0);
	return grpc::Status::OK;
}

grpc::Status OrionCommunicationServer::GotoPanaromaPixel(grpc::ServerContext *context, const OrionCommunication::TPoint *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	if (request->x() > 1.0 || request->y() > 1.0)
		return grpc::Status(StatusCode::INVALID_ARGUMENT,
							"Any request x,y coors must be less than 1.0");
	((BaseAlgorithmElement*)panaromaEl)->reloadJson();
	float panS = panaromaEl->getPanaromaControl().started.pan;
	float tiltS = panaromaEl->getPanaromaControl().started.tilt;
	float fovValue = panaromaEl->getPanaromaControl().fovValue;

	float angle = (request->x() * (360) + 10);
	if(angle > 360)
		angle -= 360;
	if(angle < 0)
		angle += 360;
	float point_x = angle;

	float theta = fovValue * 576.0 / 720.0;

	float point_y = tiltS - ((((float)request->y() - 0.5) / 0.5) * theta / 2.0);
	if (point_y > 45.0)
		point_y = 45.0;
	if (point_y < -45.0)
		point_y = -45.0;

	ffDebug() << "ReqX: %d, ReqY: %d, point_x: %d, point_y: %d" << request->x()
			  << request->y() << point_x << point_y;
	ApplicationInfo::instance()->getPtzpDriver(0)->getHead(0)->panTiltGoPos(point_x, point_y);
	return grpc::Status::OK;
}

grpc::Status OrionCommunicationServer::GetPanaromaFrames(grpc::ServerContext *context, const OrionCommunication::FrameModeQ *request, ::grpc::ServerWriter<OrionCommunication::PanoramaFrame> *writer)
{
	QElapsedTimer elaps;
	elaps.start();
	int allFrames = request->mode();
	OrionCommunication::PanoramaFrame panFrames;
	QString next = "/home/ubuntu/Desktop/Pan_images/Pan%1.jpg";
	QString nextFinal = "/home/ubuntu/Desktop/Pan_images/PanFinal.jpg";
	if (!allFrames) {
		while (1) {
			if (elaps.elapsed() > 60000 * 4)
				return Status::CANCELLED;
			if (!QFile::exists(nextFinal)) {
				sleep(1);
				continue;
			}
			QByteArray imageBuf = panaromaEl->getPanaromaFrame(nextFinal);
			panFrames.set_valid(true);
			panFrames.set_progress(100);
			panFrames.set_framedata(imageBuf.data(), imageBuf.size());
			bool ret = writer->Write(panFrames);
			if (!ret)
				return Status::CANCELLED;
			return Status::OK;
		}
	} else {
		int inc = 200;
		while (1) {
			sleep(2);
			if (elaps.elapsed() > 60000 * 4)
				return Status::CANCELLED;
			QString picture = next.arg(QString::number(inc), 4, QChar('0'));
			if (QFile::exists(nextFinal)) {
				QByteArray imageBuf = panaromaEl->getPanaromaFrame(nextFinal);
				panFrames.set_valid(true);
				panFrames.set_progress(100);
				panFrames.set_framedata(imageBuf.data(), imageBuf.size());
				bool ret = writer->Write(panFrames);
				if (!ret)
					return Status::CANCELLED;
				return Status::OK;
			} else if (QFile::exists(picture)){
				QByteArray imageBuf = panaromaEl->getPanaromaFrame(picture);
				panFrames.set_valid(true);
				panFrames.set_progress(inc / 200);
				panFrames.set_framedata(imageBuf.data(), imageBuf.size());
				bool ret = writer->Write(panFrames);
				if (!ret)
					return Status::CANCELLED;
				inc = inc + 200;
			}
		}
	}
	return Status::OK;
}

grpc::Status OrionCommunicationServer::GetMainScreenShot(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::ScreenFrame *response)
{
	if (!snapshotEl)
		return grpc::Status::CANCELLED;
	QByteArray ba = snapshotEl->take(1);
	response->set_frame(ba, ba.size());
	return grpc::Status::OK;
}

grpc::Status OrionCommunicationServer::SetVideoOverlay(grpc::ServerContext *context, const OrionCommunication::OverlayQ *request, OrionCommunication::AppCommandResult *response)
{
	// Todo
	return Status::CANCELLED;
}

OrionCommunicationServer::OrionCommunicationServer()
	:OrionCommunication::OrionCommunicationService::Service()
{
	motionEl = NULL;
	panaromaEl = NULL;
	snapshotEl = NULL;
	GrpcThreadAlg2 *grpcServer = new GrpcThreadAlg2(50060, this);
	grpcServer->start();
}
