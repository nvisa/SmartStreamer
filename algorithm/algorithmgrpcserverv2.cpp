#include "algorithmgrpcserverv2.h"
#include "libsmartelement.h"
#include "alarmsource.h"
#include "applicationinfo.h"
#include "indevicetest.h"
#include "fusionstreamer.h"
#include "applicationinfo.h"
#include "ipstreamer.h"

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

#include <QThread>
#include <QDateTime>
#include <QJsonDocument>

using namespace algorithm::v2;

class AlgorithmGrpcServerV2GrpcThreadAlg : public QThread
{
public:
	AlgorithmGrpcServerV2GrpcThreadAlg(quint16 port, AlgorithmGrpcServerV2 *s)
	{
		servicePort = port;
		algorithm = s;
	}
	void run()
	{
		std::string ep(qPrintable(QString("0.0.0.0:%1").arg(servicePort)));
		grpc::ServerBuilder builder;
		builder.AddListeningPort(ep, grpc::InsecureServerCredentials());
		builder.RegisterService(algorithm);
		std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
		server->Wait();
	}

protected:
	int servicePort;
	AlgorithmGrpcServerV2 *algorithm;
};

AlgorithmGrpcServerV2 *AlgorithmGrpcServerV2::instance()
{
	static AlgorithmGrpcServerV2 *inst = nullptr;
	if (!inst)
		inst = new AlgorithmGrpcServerV2;
	return inst;
}

void AlgorithmGrpcServerV2::addAlarmSource(QSharedPointer<AlarmSource> source)
{
	alarmSources << source;
}

AlgorithmGrpcServerV2::AlgorithmGrpcServerV2()
	: AlgorithmService::Service()
{
	AlgorithmGrpcServerV2GrpcThreadAlg *grpcServ = new AlgorithmGrpcServerV2GrpcThreadAlg(50159, this);
	grpcServ->start();
}

grpc::Status AlgorithmGrpcServerV2::GetScreenShot(grpc::ServerContext *context, const algorithm::v2::ScreenshotInfo *request, algorithm::v2::ScreenFrame *response)
{
	return grpc::Status(grpc::UNIMPLEMENTED, "Coming soon...");
}

grpc::Status AlgorithmGrpcServerV2::GetScreenShotStream(grpc::ServerContext *context, const algorithm::v2::ScreenshotInfo *request, ::grpc::ServerWriter<algorithm::v2::ScreenFrame> *writer)
{
	return grpc::Status(grpc::UNIMPLEMENTED, "Coming soon...");
}

grpc::Status AlgorithmGrpcServerV2::RunAlgorithm(grpc::ServerContext *context, const algorithm::v2::AlgorithmHead *request, google::protobuf::Empty *response)
{
	if (request->head_selection_case() != AlgorithmHead::kAlgorithmNode)
		return grpc::Status(grpc::INVALID_ARGUMENT, "Please provide a valid algorithm_node");

	std::string node = request->algorithm_node();
	if (node != "node0")
		return grpc::Status(grpc::NOT_FOUND, "No such algorithm node exists; only following are supported: node0");

	auto el = LibSmartElement::instance();
	el->setPassThru(false);

	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServerV2::RestartAlgorithm(grpc::ServerContext *context, const algorithm::v2::AlgorithmHead *request, google::protobuf::Empty *response)
{
	if (request->head_selection_case() != AlgorithmHead::kAlgorithmNode)
		return grpc::Status(grpc::INVALID_ARGUMENT, "Please provide a valid algorithm_node");

	std::string node = request->algorithm_node();
	if (node != "node0")
		return grpc::Status(grpc::NOT_FOUND, "No such algorithm node exists; only following are supported: node0");

	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServerV2::StopAlgorithm(grpc::ServerContext *context, const algorithm::v2::AlgorithmHead *request, google::protobuf::Empty *response)
{
	if (request->head_selection_case() != AlgorithmHead::kAlgorithmNode)
		return grpc::Status(grpc::INVALID_ARGUMENT, "Please provide a valid algorithm_node");

	std::string node = request->algorithm_node();
	if (node != "node0")
		return grpc::Status(grpc::NOT_FOUND, "No such algorithm node exists; only following are supported: node0");

	auto el = LibSmartElement::instance();
	el->setPassThru(true);

	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServerV2::SetAlgorithmParameters(grpc::ServerContext *context, const algorithm::v2::AlgorithmParametersSetRequest *request, google::protobuf::Empty *response)
{
	if (request->algorithm().head_selection_case() != AlgorithmHead::kAlgorithmNode)
		return grpc::Status(grpc::INVALID_ARGUMENT, "Please provide a valid algorithm_node");

	std::string node = request->algorithm().algorithm_node();
	if (node != "node0")
		return grpc::Status(grpc::NOT_FOUND, "No such algorithm node exists; only following are supported: node0");

	auto el = LibSmartElement::instance();

	switch (request->parameters_case()) {
	case AlgorithmParametersSetRequest::kPre:
		el->setPreprocessingType(request->pre());
		break;
	case AlgorithmParametersSetRequest::kVideoStabilization:
		el->setVideoStabilizationEnabled(request->video_stabilization());
		break;
	case AlgorithmParametersSetRequest::kTamperDetection:
		return grpc::Status(grpc::UNAVAILABLE, "Tamper detection cannot be modified");
		break;
	case AlgorithmParametersSetRequest::kRunMode:
		el->setRunMode(request->run_mode());
		break;
	case AlgorithmParametersSetRequest::kPreProcessingDegree:
		el->setPreprocessingDegree(request->pre_processing_degree());
		break;
	case AlgorithmParametersSetRequest::kSmartParameters: {
		auto spars = request->smart_parameters();
		if (spars.regions_size() == 0)
			return grpc::Status(grpc::INVALID_ARGUMENT, "You should at least provide (1) ROI region");
		el->setSmartMotionParameters(spars);
		break;
	}
	case AlgorithmParametersSetRequest::kPanChangeParameters:
	case AlgorithmParametersSetRequest::kDetectionParameters:
	case AlgorithmParametersSetRequest::kPrivacyMasking:
	default:
		return grpc::Status(grpc::UNAVAILABLE, "No such parameter set request implemented");
		break;
	}

	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServerV2::GetAlgorithmParameters(grpc::ServerContext *context, const algorithm::v2::AlgorithmHead *request, algorithm::v2::AlgorithmParameters *response)
{
	if (request->head_selection_case() != AlgorithmHead::kAlgorithmNode)
		return grpc::Status(grpc::INVALID_ARGUMENT, "Please provide a valid algorithm_node");

	std::string node = request->algorithm_node();
	if (node != "node0")
		return grpc::Status(grpc::NOT_FOUND, "No such algorithm node exists; only following are supported: node0");

	auto el = LibSmartElement::instance();
	response->CopyFrom(el->getParameters());

	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServerV2::ListAlgorithms(grpc::ServerContext *context, const google::protobuf::Empty *request, algorithm::v2::AlgorithmListResponse *response)
{
	auto alg = response->add_algorithms();
	alg->mutable_head_info()->set_algorithm_node("node0");
	alg->set_name("Smart algorithms");
	alg->mutable_status()->set_is_running(LibSmartElement::instance()->isPassThru());

	alg = response->add_algorithms();
	alg->mutable_head_info()->set_algorithm_node("node0");
	alg->set_name("Change detection");
	alg->mutable_status()->set_is_running(false);

	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServerV2::GetSystemFeature(grpc::ServerContext *context, const algorithm::v2::SystemFeature *request, algorithm::v2::SystemFeature *response)
{
	switch (request->feature_case()) {
	case SystemFeature::FeatureCase::kCustom: {
		auto src = request->custom();
		if (src.key() == "fusion_stream_mode") {
			auto streamers = ApplicationInfo::instance()->getStreamers();
			FusionStreamer *fstr = nullptr;
			foreach (BaseStreamer *s, streamers) {
				FusionStreamer *ip = qobject_cast<FusionStreamer *>(s);
				if (!ip)
					continue;
				fstr = ip;
				break;
			}
			if (!fstr)
				return grpc::Status(grpc::NOT_FOUND, "Fusion source is not implemented in this streamer");
			src.set_value(fstr->getMode().toStdString());
			response->mutable_custom()->CopyFrom(src);
		} else
			return grpc::Status(grpc::UNAVAILABLE, "No such feature set request implemented");
		break;
	}
		break;
	case SystemFeature::FeatureCase::kVideoSource: {
		auto streamers = ApplicationInfo::instance()->getStreamers();
		IpStreamer *ipstr = nullptr;
		foreach (BaseStreamer *s, streamers) {
			IpStreamer *ip = qobject_cast<IpStreamer *>(s);
			if (!ip)
				continue;
			ipstr = ip;
			break;
		}
		if (!ipstr)
			return grpc::Status(grpc::NOT_FOUND, "IP source is not implemented in this streamer");
		FeatureIPVideoSource *src = response->mutable_video_source();
		src->set_url(ipstr->getCurrentSource().toStdString());
		src->set_transport(FeatureIPVideoSource_Transport_UDP);
		src->set_buffer_size(0);
		break;
	}
	default:
		return grpc::Status(grpc::UNAVAILABLE, "No such feature get request implemented");
		break;
	}

	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServerV2::SetSystemFeature(grpc::ServerContext *context, const algorithm::v2::SystemFeature *request, algorithm::v2::SystemFeature *response)
{
	switch (request->feature_case()) {
	case SystemFeature::FeatureCase::kCustom: {
		auto src = request->custom();
		if (src.key() == "fusion_stream_mode") {
			auto streamers = ApplicationInfo::instance()->getStreamers();
			FusionStreamer *fstr = nullptr;
			foreach (BaseStreamer *s, streamers) {
				FusionStreamer *ip = qobject_cast<FusionStreamer *>(s);
				if (!ip)
					continue;
				fstr = ip;
				break;
			}
			if (!fstr)
				return grpc::Status(grpc::NOT_FOUND, "Fusion source is not implemented in this streamer");
			fstr->setMode(QString::fromStdString(src.value()));
		} else
			return grpc::Status(grpc::UNAVAILABLE, "No such feature set request implemented");
		break;
	}
	case SystemFeature::FeatureCase::kVideoSource: {
		auto streamers = ApplicationInfo::instance()->getStreamers();
		IpStreamer *ipstr = nullptr;
		foreach (BaseStreamer *s, streamers) {
			IpStreamer *ip = qobject_cast<IpStreamer *>(s);
			if (!ip)
				continue;
			ipstr = ip;
			break;
		}
		if (!ipstr)
			return grpc::Status(grpc::NOT_FOUND, "IP source is not implemented in this streamer");
		FeatureIPVideoSource src = request->video_source();
		ipstr->setCurrentSource(QString::fromStdString(src.url()));
		break;
	}
	default:
		return grpc::Status(grpc::UNAVAILABLE, "No such feature set request implemented");
		break;
	}

	return grpc::Status::OK;
}

grpc::Status AlgorithmGrpcServerV2::GetAlarm(grpc::ServerContext *context, ::grpc::ServerReaderWriter<algorithm::v2::Alarms, algorithm::v2::AlarmReqInfo> *stream)
{
	MultipleAlarmSource mals;
	bool sendIDTAlarms = true;

	InDeviceTest *idt = ApplicationInfo::instance()->getIDT();
	while (1) {
		Alarms res;
		AlarmReqInfo req;
		bool success = stream->Read(&req);
		if (!success)
			return grpc::Status::OK;

		std::string filter = req.filter();
		if (mals.count() == 0) {
			if (!filter.size()) {
				for (int i = 0; i < alarmSources.size(); i++)
					mals.addSource(alarmSources[i]);
			} else {
				QStringList sources = QString::fromStdString(filter).split(";");
				for (int i = 0; i < alarmSources.size(); i++)
					if (sources.contains(alarmSources[i]->typeString()))
						mals.addSource(alarmSources[i]);
				if (!sources.contains("cit"))
					sendIDTAlarms = false;
			}
		}
		auto pars = req.advancedparams();
		for (int i = 0; i < alarmSources.size(); i++)
			alarmSources[i]->setParameters(pars);
		int32_t interval = req.intervalmsecs();
		res.set_ts(QDateTime::currentMSecsSinceEpoch());

		/* JIT alarms still don't use alarmsource */
		if (idt && sendIDTAlarms) {
			QJsonObject cit = idt->getLastCheckResults();
			if (cit.contains("faults")) {
				Alarm *alarm = res.add_alarms();
				alarm->set_type("cit");
				alarm->add_key("details");
				QByteArray alarmArr = QJsonDocument(cit).toJson();
				alarm->add_value(std::string(alarmArr.data(), alarmArr.size()));
			}
		}

		auto list = mals.wait(interval);
		for (int i = 0; i < list.size(); i++) {
			const auto source = list[i];
			Alarm *alarm = res.add_alarms();
			alarm->set_type(source->typeString().toStdString());
			AlarmSource::QueueData qd = source->fetch();
			const QHash<QString, QVariant> &list = qd.hash;
			foreach (const algorithm::v2::DetectedObject &obj, qd.detectedObjects) {
				auto o = alarm->add_detected_object();
				o->CopyFrom(obj);
			}
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

		success = stream->Write(res);
		if (!success)
			return grpc::Status::OK;
	}

	return grpc::Status::OK;
}
