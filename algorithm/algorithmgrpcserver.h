#ifndef ALGORITHMGRPCSERVER_H
#define ALGORITHMGRPCSERVER_H

#include <QObject>
#include <QJsonObject>
#include <QProcess>

#include "snapshotelement.h"
#include "proto/AlgorithmCommunication.grpc.pb.h"

#include "algorithm/facealgorithmelement.h"
#include "algorithm/trackalgorithmelement.h"
#include "algorithm/motionalgorithmelement.h"
#include "algorithm/panaromaalgorithmelement.h"
#include "algorithm/stabilizationalgorithmelement.h"
#include "algorithm/panchangealgorithmelement.h"

class AlgoManIface
{
public:
	virtual int runAlgorithm(int channel) = 0;
	virtual int stopAlgorithm(int channel) = 0;
};

class AlgorithmGrpcServer : public AlgorithmCommunication::AlgorithmService::Service
{
public:
	static AlgorithmGrpcServer * instance();
	void setAlgorithmManagementInterface(AlgoManIface *i);
	void setFaceAlgorithmElement(FaceAlgorithmElement *el) {faceEl = el;}
	void setTrackAlgorithmElement(TrackAlgorithmElement *el) {trackEl = el;}
	void setMotionAlgorithmElement(MotionAlgorithmElement *el) {motionEl = el;}
	void setPanaromaAlgorithmElement(PanaromaAlgorithmElement *el) {panaromaEl = el;}
	void setStabilizationAlgorithmElement(StabilizationAlgorithmElement *el) {stabilizationEl = el;}
	void setPanChangeAlgorithmElement(PanChangeAlgorithmElement *el) {panChangeEl = el;}
	void setSnapshotElement(SnapshotElement *el) {snapshotEl = el;}
	void setAlarmField(const QString &key, const QString &value);
	void removeAlarmField(const QString &key);
	grpc::Status RunAlgorithm(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response);
	grpc::Status StopAlgorithm(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response);

	grpc::Status SetAlgorithmParameters(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response);
	grpc::Status GetAlgorithmParameters(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfParameterRequests *response);

	grpc::Status GetScreenShot(grpc::ServerContext *context, const AlgorithmCommunication::ScreenshotInfo *request, AlgorithmCommunication::ScreenFrame *response);
	grpc::Status GetScreenShotStream(grpc::ServerContext *context, const AlgorithmCommunication::ScreenshotInfo *request, ::grpc::ServerWriter<AlgorithmCommunication::ScreenFrame> *writer);

	grpc::Status GetSystemFeature(grpc::ServerContext *context, const AlgorithmCommunication::SystemFeature *request, AlgorithmCommunication::SystemFeature *response);
	grpc::Status SetSystemFeature(grpc::ServerContext *context, const AlgorithmCommunication::SystemFeature *request, AlgorithmCommunication::SystemFeature *response);
	void enableTrackAutoStopping();
	void ptzCommandRecved(int cmd);

protected:
	explicit AlgorithmGrpcServer(QObject *parent = 0);
	int setMotionParameters(MotionAlgorithmElement *el, AlgorithmCommunication::MotionParameters p);
	int getMotionParameters(AlgorithmCommunication::MotionParameters p);
	BaseAlgorithmElement *algorithmElementManager(int chn);
	int setTrackParameters(TrackAlgorithmElement *el, AlgorithmCommunication::TrackParameters p);
	int setStabilizationParameters(StabilizationAlgorithmElement *el, AlgorithmCommunication::StabilizationParameters p);
	int setPanChangeParameters(PanChangeAlgorithmElement *el, AlgorithmCommunication::PanChangeParameters &p);
private:
	FaceAlgorithmElement *faceEl;
	TrackAlgorithmElement *trackEl;
	MotionAlgorithmElement *motionEl;
	PanaromaAlgorithmElement *panaromaEl;
	StabilizationAlgorithmElement *stabilizationEl;
	PanChangeAlgorithmElement *panChangeEl;
	SnapshotElement *snapshotEl;
	AlgoManIface *manif;
	QMutex mutex;
	QHash<QString, QString> alarms;

	// Service interface
public:
	grpc::Status GetAlarm(grpc::ServerContext *context, ::grpc::ServerReaderWriter<AlgorithmCommunication::Alarms, AlgorithmCommunication::AlarmReqInfo> *stream) override;
};

#endif // ALGORITHMGRPCSERVER_H
