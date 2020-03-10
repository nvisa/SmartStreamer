#ifndef ALGORITHMGRPCSERVERV2_H
#define ALGORITHMGRPCSERVERV2_H

#include <QMutex>

#include "alarmsource.h"
#include "snapshotelement.h"
#include "proto/v2/AlgorithmCommunicationV2.grpc.pb.h"

class AlgorithmGrpcServerV2 : public algorithm::v2::AlgorithmService::Service
{
public:
	static AlgorithmGrpcServerV2 * instance();

	void addAlarmSource(QSharedPointer<AlarmSource> source);
	void setPanChangeFrame(const std::string &tag, const QByteArray &image);
protected:

	AlgorithmGrpcServerV2();

	// Service interface
public:
	grpc::Status GetScreenShot(grpc::ServerContext *context, const algorithm::v2::ScreenshotInfo *request, algorithm::v2::ScreenFrame *response) override;
	grpc::Status GetScreenShotStream(grpc::ServerContext *context, const algorithm::v2::ScreenshotInfo *request, ::grpc::ServerWriter<algorithm::v2::ScreenFrame> *writer) override;
	grpc::Status RunAlgorithm(grpc::ServerContext *context, const algorithm::v2::AlgorithmHead *request, google::protobuf::Empty *response) override;
	grpc::Status RestartAlgorithm(grpc::ServerContext *context, const algorithm::v2::AlgorithmHead *request, google::protobuf::Empty *response) override;
	grpc::Status StopAlgorithm(grpc::ServerContext *context, const algorithm::v2::AlgorithmHead *request, google::protobuf::Empty *response) override;
	grpc::Status SetAlgorithmParameters(grpc::ServerContext *context, const algorithm::v2::AlgorithmParametersSetRequest *request, google::protobuf::Empty *response) override;
	grpc::Status GetAlgorithmParameters(grpc::ServerContext *context, const algorithm::v2::AlgorithmHead *request, algorithm::v2::AlgorithmParameters *response) override;
	grpc::Status ListAlgorithms(grpc::ServerContext *context, const google::protobuf::Empty *request, algorithm::v2::AlgorithmListResponse *response) override;
	grpc::Status GetSystemFeature(grpc::ServerContext *context, const algorithm::v2::SystemFeature *request, algorithm::v2::SystemFeature *response) override;
	grpc::Status SetSystemFeature(grpc::ServerContext *context, const algorithm::v2::SystemFeature *request, algorithm::v2::SystemFeature *response) override;
	grpc::Status GetAlarm(grpc::ServerContext *context, ::grpc::ServerReaderWriter<algorithm::v2::Alarms, algorithm::v2::AlarmReqInfo> *stream) override;

	void setSnapshotElement(SnapshotElement *el) {snapshotEl = el;}

protected:
	QList<QSharedPointer<AlarmSource>> alarmSources;
	QByteArray lastImage;
	std::string lastImgTag;
	QMutex mutex;
	SnapshotElement *snapshotEl;
};

#endif // ALGORITHMGRPCSERVERV2_H
