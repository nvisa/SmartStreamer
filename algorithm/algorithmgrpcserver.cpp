#include "algorithmgrpcserver.h"
#include "diagnosticserviceimpl.h"
#include "indevicetest.h"
#include "streamercommon.h"

#include <ecl/drivers/patternng.h>

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

#include <QImage>
#include <QBuffer>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>

#include <unistd.h>

#define returnOnError(x) \
	if(!x) { \
		qDebug() << "Algorithm elemenet didn't found :" << x;\
		return grpc::Status::CANCELLED; \
	}

class AlgorithmGrpcServerGrpcThreadAlg : public QThread
{
public:
	AlgorithmGrpcServerGrpcThreadAlg(quint16 port, AlgorithmGrpcServer *s)
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

class PtzNotification : public PatternNg
{
public:
	PtzNotification(AlgorithmGrpcServer *s, PtzpDriver *driver)
		: PatternNg(driver)
	{
		this->s = s;
	}

	virtual void commandUpdate(int pan, int tilt, int zoom, int c, float par1, float par2)
	{
		s->ptzCommandRecved(c);
		PatternNg::commandUpdate(pan, tilt, zoom, c, par1, par2);
	}

	AlgorithmGrpcServer *s;
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
	panChangeEl = NULL;
	snapshotEl = NULL;
	manif = NULL;

	AlgorithmGrpcServerGrpcThreadAlg *grpcServ = new AlgorithmGrpcServerGrpcThreadAlg(50059, this);
	grpcServ->start();
	enableTrackAutoStopping();
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

AlgoManIface *AlgorithmGrpcServer::getAlgorithmManagementInterface()
{
	return manif;
}

void AlgorithmGrpcServer::removeAlarmField(const QString &alarm, const QString &key)
{
	QMutexLocker ml(&mutex);
	alarms[alarm].remove(key);
}

void AlgorithmGrpcServer::setAlarmField(const QString &alarm, const QString &key, const QString &value)
{
	QMutexLocker ml(&mutex);
	alarms[alarm][key] = QVariant(value);
}

void AlgorithmGrpcServer::setAlarmField(const QString &alarm, const QString &key, const QVariant &value)
{
	QMutexLocker ml(&mutex);
	alarms[alarm][key] = value;
}

void AlgorithmGrpcServer::removeAlarm(const QString &alarm)
{
	QMutexLocker ml(&mutex);
	alarms.remove(alarm);
}

grpc::Status AlgorithmGrpcServer::RunAlgorithm(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response)
{
	Q_UNUSED(context);
	AlgorithmCommunication::TrackParameters trackpar = request->trackparam();
	AlgorithmCommunication::MotionParameters motionpar = request->motionparam();
	AlgorithmCommunication::PanaromaParameters panaromapar = request->panaromaparam();
	AlgorithmCommunication::StabilizationParameters stabilizationpar = request->stabilizationparam();
	AlgorithmCommunication::FaceDetectionParameters facedetectionpar = request->facedetectionparam();
	AlgorithmCommunication::PanChangeParameters panchangepar = request->panchangeparam();

	if (manif) {
		int err = manif->runAlgorithm(request->channel());
		if (!err)
			return grpc::Status::OK;
		if (err != -EAGAIN)
			return grpc::Status::CANCELLED;
	}
	BaseAlgorithmElement *el = algorithmElementManager(request->channel());
	returnOnError(el);

	switch (request->algorithmtype()) {
	case AlgorithmCommunication::RequestForAlgorithm::MOTION:
		el->setState(BaseAlgorithmElement::INIT);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::STABILIZATION:
		el->setState(BaseAlgorithmElement::INIT);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::TRACKING:
		el->setState(BaseAlgorithmElement::INIT);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::PANAROMA:
		el->setState(BaseAlgorithmElement::INIT);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::FACE_DETECTION:
		break;
	case AlgorithmCommunication::RequestForAlgorithm::PAN_CHANGE:
		el->setState(BaseAlgorithmElement::INIT);
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
	AlgorithmCommunication::PanChangeParameters panchangepar = request->panchangeparam();

	if (manif) {
		int err = manif->stopAlgorithm(request->channel());
		if (!err)
			return grpc::Status::OK;
		if (err != -EAGAIN)
			return grpc::Status::CANCELLED;
	}
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
	case AlgorithmCommunication::RequestForAlgorithm::PAN_CHANGE:
		el->setState(BaseAlgorithmElement::STOPALGO);
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
	AlgorithmCommunication::PanChangeParameters panchangepar = request->panchangeparam();

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
		return setTrackParameters((TrackAlgorithmElement*)el, trackpar);
		break;
	case AlgorithmCommunication::RequestForAlgorithm::PANAROMA:
		break;
	case AlgorithmCommunication::RequestForAlgorithm::FACE_DETECTION:
		break;
	case AlgorithmCommunication::RequestForAlgorithm::PAN_CHANGE:
		setPanChangeParameters((PanChangeAlgorithmElement*)el, panchangepar);
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

grpc::Status AlgorithmGrpcServer::GetSystemFeature(grpc::ServerContext *context, const AlgorithmCommunication::SystemFeature *request, AlgorithmCommunication::SystemFeature *response)
{
	AlgorithmCommunication::SystemFeature *resp = response;
	if (request->key() == "LifeTime") {
		qint64 lifeTime = ApplicationInfo::instance()->getLifeTime();
		response->set_value(QString("%1").arg(lifeTime).toStdString());
		response->set_key("LifeTime");
		return grpc::Status::OK;
	} else if (request->key() == "Bitrate") {
		QJsonArray arr = StreamerCommon::readSettingsJSON("/etc/smartstreamer/encoders.json").array();
		if (arr.size() <= 0)
			return grpc::Status::CANCELLED;
		int device = request->device();
		if (device >= arr.size())
			return grpc::Status(StatusCode::NOT_FOUND, request->key());
		QJsonObject obj = arr[device].toObject();
		response->set_value(QString("%1").arg(obj["bitrate"].toInt()).toStdString());
		response->set_key("Bitrate");
		response->set_device(device);
		return grpc::Status::OK;
	}	else if (request->key() == "FrameRate") {
		QJsonArray arr = StreamerCommon::readSettingsJSON("/etc/smartstreamer/encoders.json").array();
		if (arr.size() <= 0)
			return grpc::Status::CANCELLED;
		int device = request->device();
		if (device >= arr.size())
			return grpc::Status(StatusCode::NOT_FOUND, request->key());
		QJsonObject obj = arr[device].toObject();
		response->set_value(QString("%1").arg(obj["frameRate"].toInt()).toStdString());
		response->set_key("FrameRate");
		response->set_device(device);
		return grpc::Status::OK;
	}	else if (request->key() == "Resolution") {
		QJsonArray arr = StreamerCommon::readSettingsJSON("/etc/smartstreamer/encoders.json").array();
		if (arr.size() <= 0)
			return grpc::Status::CANCELLED;
		int device = request->device();
		if (device >= arr.size())
			return grpc::Status(StatusCode::NOT_FOUND, request->key());
		QJsonObject obj = arr[device].toObject();
		response->set_value(QString("%1").arg(obj["resolution"].toInt()).toStdString());
		response->set_key("Resolution");
		response->set_device(device);
		return grpc::Status::OK;
	}
	return grpc::Status(StatusCode::NOT_FOUND, request->key());
}

grpc::Status AlgorithmGrpcServer::SetSystemFeature(grpc::ServerContext *context, const AlgorithmCommunication::SystemFeature *request, AlgorithmCommunication::SystemFeature *response)
{
	if (request->key() == "Bitrate") {
		QJsonDocument doc = StreamerCommon::readSettingsJSON("/etc/smartstreamer/encoders.json");
		QJsonArray arr = doc.array();
		if (arr.size() <= 0)
			return grpc::Status::CANCELLED;
		int device = request->device();
		QJsonObject obj = arr[device].toObject();
		int btr = atoi(request->value().c_str());
		if (obj["bitrate_min"].toInt() > btr || obj["bitrate_max"].toInt() < btr)
			return grpc::Status(StatusCode::INVALID_ARGUMENT, request->value());
		obj["bitrate"] = btr;
		arr[device] = obj;
		QJsonDocument docToSave(arr);
		StreamerCommon::writeSettingsJSON("/etc/smartstreamer/encoders.json", docToSave);
		response->set_value(request->value());
		response->set_key("Bitrate");
		response->set_device(device);
		return grpc::Status::OK;
	} else if (request->key() == "FrameRate") {
		QJsonDocument doc = StreamerCommon::readSettingsJSON("/etc/smartstreamer/encoders.json");
		QJsonArray arr = doc.array();
		if (arr.size() <= 0)
			return grpc::Status::CANCELLED;
		int device = request->device();
		QJsonObject obj = arr[device].toObject();
		int fps = atoi(request->value().c_str());
		if (obj["framerate_min"].toInt() > fps || obj["framerate_max"].toInt() < fps)
			return grpc::Status(StatusCode::INVALID_ARGUMENT, request->value());
		obj["frameRate"] = fps;
		arr[device] = obj;
		QJsonDocument docToSave(arr);
		StreamerCommon::writeSettingsJSON("/etc/smartstreamer/encoders.json", docToSave);
		response->set_value(request->value());
		response->set_key("FrameRate");
		response->set_device(device);
		return grpc::Status::OK;
	} else if (request->key() == "Resolution") {
		QJsonDocument doc = StreamerCommon::readSettingsJSON("/etc/smartstreamer/encoders.json");
		QJsonArray arr = doc.array();
		if (arr.size() <= 0)
			return grpc::Status::CANCELLED;
		int device = request->device();
		QJsonObject obj = arr[device].toObject();
		QString resolution = QString::fromStdString(request->value());
		QJsonArray array = obj["resolutions"].toArray();
		if (!array.contains(resolution))
			return grpc::Status(StatusCode::INVALID_ARGUMENT, request->value());
		obj["resolution"] = resolution;
		arr[device] = obj;
		QJsonDocument docToSave(arr);
		StreamerCommon::writeSettingsJSON("/etc/smartstreamer/encoders.json", docToSave);
		response->set_value(request->value());
		response->set_key("Resolution");
		response->set_device(device);
		return grpc::Status::OK;
	} else if (request->key() == "Reboot") {
		QProcess::startDetached("reboot");
		return grpc::Status::OK;
	} else if (request->key() == "Reset") {
		QProcess::startDetached("killall SmartStreamer");
		return grpc::Status::OK;
	}  else if (request->key() == "grabPanTilt") {
		if (request->value() == "disable") {
			ApplicationInfo::instance()->getPtzpDriver(0)->enableDriver(false);
			return grpc::Status(StatusCode::OK, "Pan-Tilt is disabled");
		}
		ApplicationInfo::instance()->getPtzpDriver(0)->enableDriver(true);
		return grpc::Status::OK;
	}
	return grpc::Status(StatusCode::NOT_FOUND, "Request is not found");
}

void AlgorithmGrpcServer::enableTrackAutoStopping()
{
	PtzpDriver *ptzp = ApplicationInfo::instance()->getPtzpDriver(0);
	ptzp->setPatternHandler(new PtzNotification(this, ptzp));
}

void AlgorithmGrpcServer::ptzCommandRecved(int cmd)
{
	//zoom stop -> 6     pan-tilt -> stop
	if (cmd == 7 || cmd == 6) {
		//TODO: decide which algorithm to start (if any)
	} else {
		BaseAlgorithmElement *el = algorithmElementManager(1);
		if (el->getState() == BaseAlgorithmElement::PROCESS) {
			if (cmd == 4 || cmd == 5) {
				//TODO: do not hardcode PT head
				ApplicationInfo::instance()->getPtzpDriver(0)->getHead(1)->panTiltStop();
			}
			el->setState(BaseAlgorithmElement::STOPALGO);
		}
	}
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

grpc::Status AlgorithmGrpcServer::setTrackParameters(TrackAlgorithmElement *el, AlgorithmCommunication::TrackParameters p)
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
		float var = p.target().point_x();
		if (var < 0) {
			return grpc::Status(grpc::OUT_OF_RANGE, "Negative point_x.");
		}
		var = p.target().point_y();
		if (var < 0) {
			return grpc::Status(grpc::OUT_OF_RANGE, "Negative point_y.");
		}
		var = p.target().width();
		if (var < 0) {
			return grpc::Status(grpc::OUT_OF_RANGE, "Negative width.");
		}
		var = p.target().height();
		if (var < 0) {
			return grpc::Status(grpc::OUT_OF_RANGE, "Negative height.");
		}

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
	return grpc::Status::OK;
}

int AlgorithmGrpcServer::setStabilizationParameters(StabilizationAlgorithmElement *el, AlgorithmCommunication::StabilizationParameters p)
{
	el->setPrivacy(p.privacy());
	el->setStabilization(p.stabilization());
	el->setSensitivity(p.sensitivity());
	el->savetoJson();
	return 0;
}

int AlgorithmGrpcServer::setPanChangeParameters(PanChangeAlgorithmElement *el, AlgorithmCommunication::PanChangeParameters &p)
{
	el->setPanChangeInfoFrom(p.locationinformation());
	el->savetoJson();
	return 0;
}

BaseAlgorithmElement* AlgorithmGrpcServer::algorithmElementManager(int chn)
{
	return ApplicationInfo::instance()->getAlgorithmInstance(chn);
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
					if (obj["type"].toString() == "control_module" && obj["index"].toInt() == 0)
						citinfo->set_moduleinfo(AlgorithmCommunication::CITInfo_Info_NOK);
					if (obj["type"].toString() == "control_module" && obj["index"].toInt() == 1)
						citinfo->set_motorinfo(AlgorithmCommunication::CITInfo_Info_NOK);
					if (obj["type"].toString() == "video_module")
						citinfo->set_usbinfo(AlgorithmCommunication::CITInfo_Info_NOK);
				}
				alarm->add_key("details");
				QByteArray alarmArr =  QJsonDocument(cit).toJson();
				alarm->add_value(std::string(alarmArr.data(), alarmArr.size()));
			}
		}

		mutex.lock();
		QHashIterator<QString, QHash<QString, QVariant> > hi(alarms);
		while (hi.hasNext()) {
			hi.next();
			const QString name = hi.key();
			AlgorithmCommunication::Alarm *alarm = res.add_alarms();
			alarm->set_type(name.toStdString());
			const QHash<QString, QVariant> &list = hi.value();
			QHashIterator<QString, QVariant> hi2(list);
			while (hi2.hasNext()) {
				hi2.next();
				const QString key = hi2.key();
				const QVariant value = hi2.value();
				alarm->add_key(key.toStdString());
				if (value.canConvert(QVariant::ByteArray)) {
					const QByteArray &ba = value.toByteArray();
					std::string str(ba.constData(), ba.size());
					alarm->add_value(str);
				} else if (value.canConvert(QVariant::String)) {
					alarm->add_value(value.toString().toStdString());
				} else {
					alarm->add_value("N/A");
				}
			}
		}
		mutex.unlock();

		success = stream->Write(res);
		if (!success)
			return Status::OK;
	}

	return Status::OK;
}
