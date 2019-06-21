#include "algorithmgrpcserver.h"
#include "diagnosticserviceimpl.h"
#include "indevicetest.h"

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
#include <QDateTime>

#include <unistd.h>

#define returnOnError(x) \
	if(!x) { \
		qDebug() << "Algorithm elemenet didn't found :%d", x;\
		return grpc::Status::CANCELLED; \
	}

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

void AlgorithmGrpcServer::setAlgorithmManagementInterface(AlgoManIface *i)
{
	manif = i;
}

void AlgorithmGrpcServer::setAlarmField(const QString &key, const QString &value)
{
	QMutexLocker ml(&mutex);
	alarms[key] = value;
}

void AlgorithmGrpcServer::removeAlarmField(const QString &key)
{
	QMutexLocker ml(&mutex);
	alarms.clear();
}

grpc::Status AlgorithmGrpcServer::RunAlgorithm(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response)
{
	Q_UNUSED(context);
	AlgorithmCommunication::TrackParameters trackpar = request->trackparam();
	AlgorithmCommunication::MotionParameters motionpar = request->motionparam();
	AlgorithmCommunication::PanaromaParameters panaromapar = request->panaromaparam();
	AlgorithmCommunication::StabilizationParameters stabilizationpar = request->stabilizationparam();
	AlgorithmCommunication::FaceDetectionParameters facedetectionpar = request->facedetectionparam();

	BaseAlgorithmElement *el = algorithmElementManager(request->channel());
	returnOnError(el);

	switch (request->algorithmtype()) {
	case AlgorithmCommunication::RequestForAlgorithm::MOTION:
		if (!setMotionParameters((MotionAlgorithmElement*)el, motionpar))
			el->setState(BaseAlgorithmElement::INIT);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::STABILIZATION:
		if (!setStabilizationParameters((StabilizationAlgorithmElement*)el, stabilizationpar))
			el->setState(BaseAlgorithmElement::INIT);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::TRACKING:
		if (!setTrackParameters((TrackAlgorithmElement*)el, trackpar))
			el->setState(BaseAlgorithmElement::INIT);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::PANAROMA:
		el->setState(BaseAlgorithmElement::INIT);
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
	Q_UNUSED(context);
	AlgorithmCommunication::TrackParameters trackpar = request->trackparam();
	AlgorithmCommunication::MotionParameters motionpar = request->motionparam();
	AlgorithmCommunication::PanaromaParameters panaromapar = request->panaromaparam();
	AlgorithmCommunication::StabilizationParameters stabilizationpar = request->stabilizationparam();
	AlgorithmCommunication::FaceDetectionParameters facedetectionpar = request->facedetectionparam();

	BaseAlgorithmElement *el = algorithmElementManager(request->channel());
	returnOnError(el);

	switch (request->algorithmtype()) {
	case AlgorithmCommunication::RequestForAlgorithm::MOTION:
		el->setState(BaseAlgorithmElement::STOPALGO);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::STABILIZATION:
		el->setState(BaseAlgorithmElement::STOPALGO);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::TRACKING:
		el->setState(BaseAlgorithmElement::STOPALGO);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::PANAROMA:
		el->setState(BaseAlgorithmElement::STOPALGO);
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
	Q_UNUSED(context);
	AlgorithmCommunication::TrackParameters trackpar = request->trackparam();
	AlgorithmCommunication::MotionParameters motionpar = request->motionparam();
	AlgorithmCommunication::PanaromaParameters panaromapar = request->panaromaparam();
	AlgorithmCommunication::StabilizationParameters stabilizationpar = request->stabilizationparam();
	AlgorithmCommunication::FaceDetectionParameters facedetectionpar = request->facedetectionparam();

	BaseAlgorithmElement *el = algorithmElementManager(request->channel());
	returnOnError(el);

	switch (request->algorithmtype()) {
	case AlgorithmCommunication::RequestForAlgorithm::MOTION:
		setMotionParameters((MotionAlgorithmElement *)el, motionpar);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::STABILIZATION: {
		setStabilizationParameters((StabilizationAlgorithmElement *)el, stabilizationpar);
		break;
	}
	case AlgorithmCommunication::RequestForAlgorithm::TRACKING:
		setTrackParameters((TrackAlgorithmElement*)el, trackpar);
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
	Q_UNUSED(context);
	AlgorithmCommunication::TrackParameters *tp;
	AlgorithmCommunication::MotionParameters *mp;
	AlgorithmCommunication::StabilizationParameters *sp;

	BaseAlgorithmElement *el = algorithmElementManager(request->channel());
	returnOnError(el);

	switch (request->algorithmtype()) {
	case AlgorithmCommunication::RequestForAlgorithm::MOTION: {
		MotionAlgorithmElement* mo = (MotionAlgorithmElement*) el;
		mp = mo->loadROI();
		mp->set_sensitivity(mo->getSensitivity());
		response->set_allocated_motionparam(mp);
		break;
	}
	case AlgorithmCommunication::RequestForAlgorithm::STABILIZATION: {
		StabilizationAlgorithmElement* sa = (StabilizationAlgorithmElement*) el;
		sp = new AlgorithmCommunication::StabilizationParameters();
		sp->set_privacy(sa->getPrivacy());
		sp->set_sensitivity(sa->getSensitivity());
		sp->set_stabilization(sa->getStabilization());
		response->set_allocated_stabilizationparam(sp);
		break;
	}
	case AlgorithmCommunication::RequestForAlgorithm::TRACKING: {
		TrackAlgorithmElement* ta = (TrackAlgorithmElement*) el;
		tp = new AlgorithmCommunication::TrackParameters();
		tp->set_trackscore(ta->getTrackScore());
		tp->set_trackinterval(ta->getTrackInterval());
		float* objInfo = ta->getTrackObjInfo();
		AlgorithmCommunication::TrackObject *to = new AlgorithmCommunication::TrackObject();
		to->set_point_x(objInfo[0]);
		to->set_point_y(objInfo[1]);
		to->set_width(objInfo[2]);
		to->set_height(objInfo[3]);
		tp->set_allocated_target(to);
		response->set_allocated_trackparam(tp);
		break;
	}
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

int AlgorithmGrpcServer::setMotionParameters(MotionAlgorithmElement *el, AlgorithmCommunication::MotionParameters p)
{
	int ret = 0;
	switch (p.settingchoice()) {
	case AlgorithmCommunication::MotionParameters::SENSITIVITY:
		el->setSensitivity(p.sensitivity());
		break;
	case AlgorithmCommunication::MotionParameters::ROI:
		ret = el->saveROI(p.roilist());
		break;
	case AlgorithmCommunication::MotionParameters::BOTH:
		el->setSensitivity(p.sensitivity());
		ret = el->saveROI(p.roilist());
		break;
	default:
		break;
	}
	el->savetoJson();
	return ret;
}

int AlgorithmGrpcServer::setTrackParameters(TrackAlgorithmElement *el, AlgorithmCommunication::TrackParameters p)
{
	switch (p.tracktype()) {
	case AlgorithmCommunication::TrackParameters::AUTO:
		el->setMode(TrackAlgorithmElement::AUTO);
		el->setTrackStyle(p.mode());
		break;
	case AlgorithmCommunication::TrackParameters::SEMI_AUTO:
		// el->setMode(TrackAlgorithmElement::SEMIAUTO);
		break;
	case AlgorithmCommunication::TrackParameters::MANUAL: {
		el->setMode(TrackAlgorithmElement::MANUAL);
		el->setTrackObjInfo(p.target().point_x(), p.target().point_y(), p.target().width(), p.target().height());
		break;
	}
	default:
		break;
	}
	el->setTrackInterval(p.trackinterval());
	el->setTrackScore(p.trackscore());
	el->savetoJson();
	return 0;
}

int AlgorithmGrpcServer::setStabilizationParameters(StabilizationAlgorithmElement *el, AlgorithmCommunication::StabilizationParameters p)
{
	el->setPrivacy(p.privacy());
	el->setStabilization(p.stabilization());
	el->setSensitivity(p.sensitivity());
	el->savetoJson();
	return 0;
}

BaseAlgorithmElement* AlgorithmGrpcServer::algorithmElementManager(int chn)
{
	BaseAlgorithmElement *el = nullptr;
	if (manif)
		el = manif->getAlgo(chn);
	return el;
}

grpc::Status AlgorithmGrpcServer::GetAlarm(grpc::ServerContext *context, ::grpc::ServerReaderWriter<AlgorithmCommunication::Alarms, AlgorithmCommunication::AlarmReqInfo> *stream)
{
	InDeviceTest *idt = ApplicationInfo::instance()->getIDT();
	while (1) {
		AlgorithmCommunication::Alarms res;
		AlgorithmCommunication::AlarmReqInfo req;
		bool success = stream->Read(&req);
		if (!success)
			return Status::OK;

		std::string filter = req.filter();
		int32_t interval = req.intervalmsecs();
		usleep(interval * 1000);
		res.set_ts(QDateTime::currentMSecsSinceEpoch());
		if (idt) {
			QJsonObject cit = idt->getLastCheckResults();
			if (cit.contains("faults")) {
				AlgorithmCommunication::Alarm *alarm = res.add_alarms();
				alarm->set_type("cit");
				AlgorithmCommunication::CITInfo *citinfo = alarm->mutable_cit();
				citinfo->set_moduleinfo(AlgorithmCommunication::CITInfo_Info_OK);
				citinfo->set_motorinfo(AlgorithmCommunication::CITInfo_Info_OK);
				citinfo->set_usbinfo(AlgorithmCommunication::CITInfo_Info_OK);
				foreach (QJsonValue v, cit["faults"].toArray()) {
					QJsonObject obj = v.toObject();
					if (obj["type"] == "control_module" && obj["index"].toInt() == 0)
						citinfo->set_moduleinfo(AlgorithmCommunication::CITInfo_Info_NOK);
					if (obj["type"] == "control_module" && obj["index"].toInt() == 1)
						citinfo->set_motorinfo(AlgorithmCommunication::CITInfo_Info_NOK);
					if (obj["type"] == "video_module")
						citinfo->set_usbinfo(AlgorithmCommunication::CITInfo_Info_NOK);
				}
			}
		}

		mutex.lock();
		if (alarms.size()) {
			AlgorithmCommunication::Alarm *alarm = res.add_alarms();
			alarm->set_type("motion_detection");
			QHashIterator<QString, QString> ait(alarms);
			while (ait.hasNext()) {
				ait.next();
				alarm->add_key(ait.key().toStdString());
				alarm->add_value(ait.value().toStdString());
			}
		}
		mutex.unlock();

		success = stream->Write(res);
		if (!success)
			return Status::OK;
	}

	return Status::OK;
}
