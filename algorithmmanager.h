#ifndef ALGORITHMMANAGER_H
#define ALGORITHMMANAGER_H

#include "lmm/baselmmelement.h"

#include "ecl/ptzp/ptzphead.h"
#include "ecl/debug.h"
#include "algorithmelement.h"
#include "commoninformationinterface.h"
#include "proto/AlgorithmCommunication.grpc.pb.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileSystemWatcher>

class QTimer;
class TbgthDriver;
class AryaDriver;
class IRDomeDriver;
class RawBuffer;
class PtzpHead;
class KayiDriver;
class PtzpDriver;
class GrpcThread;
class AlgorithmElement;

class AlgorithmManager : public AlgorithmCommunication::AlgorithmService::Service
{
public:
	//GRPC related functions
	grpc::Status RunAlgorithm(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response);
	grpc::Status StopAlgorithm(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response);
	grpc::Status SetAlgorithmParameters(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response);
	grpc::Status GetAlgorithmParameters(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfParameterRequests *response);
	grpc::Status SetSystemFeatures(grpc::ServerContext *context, const AlgorithmCommunication::RequestForSystemFeatures *request, AlgorithmCommunication::ResponseOfSystemFeatures *response);
	grpc::Status GetSystemFeatures(grpc::ServerContext *context, const AlgorithmCommunication::RequestForSystemFeatures *request, AlgorithmCommunication::ResponseOfSystemFeatures *response);
	grpc::Status SendInformation(::grpc::ServerContext* context, const ::AlgorithmCommunication::WhichInfo* request, ::grpc::ServerWriter< ::AlgorithmCommunication::GeneralInfo>* writer);

	enum Algorithm {
		MOTION,
		STABILIZATION,
		TRACKING,
		PANAROMA,
		FACE_DETECTION,
		NONE,
	};
	enum System {
		BOTAS_FIX,
		BOTAS_DOME,
		TBGTH,
		ARYA,
		KAYI,
		TEST,
		EMPTY_SYSTEM,
	};

	explicit AlgorithmManager(QObject *parent);
	~AlgorithmManager();
	void startGrpc();
	QMap<Algorithm,bool> availableAlgortihms;
	AlgorithmElement::AlgorithmElementHandler algHandlerEl;
	AlgorithmElement::AlgorithmElementHandler getAlgHandlerFor(int index);
	void registerAlgorithm(Algorithm alg, AlgorithmElement *el);
	const AlgorithmElement * getAlgorithmElement(Algorithm);
	int addAlarm(const AlgorithmCommunication::AlarmInfo &info);
	AlarmInfo getAlarmElement(){return alarmInfo;}
	void updatePTInfo();
	int setPT(PTZinformation info);
	int setZoom(uint pos);
private:
	int openAlgRelatedJson();
	int updateParametersFromJson();
	bool checkSystemParameters();

	//GRPC Handler Function Return Structure
	struct GRPCSetReturn {
		AlgorithmElement *el;
		AlgorithmCommunication::ResponseOfRequests *response;
		grpc::StatusCode status;
	};
	GRPCSetReturn gsr;
	//GRPC Handler Functions
	GRPCSetReturn handleMotionDetection(AlgorithmElement *, const AlgorithmCommunication::MotionParameters::Settings, AlgorithmCommunication::ResponseOfRequests *response, const ::AlgorithmCommunication::TRoi, int sensitivity = 0);
	GRPCSetReturn handleTracking(AlgorithmElement *, const AlgorithmCommunication::TrackParameters::TrackType, const AlgorithmCommunication::TrackParameters::Mode, AlgorithmCommunication::ResponseOfRequests *response, const ::AlgorithmCommunication::TrackObject, float trackScore = 0.5, int trackInterval = 30);
	GRPCSetReturn handleStabilization(AlgorithmElement *, const AlgorithmCommunication::StabilizationParameters, AlgorithmCommunication::ResponseOfRequests *response, int sensitivity = 0);
	GRPCSetReturn handlePanaroma(AlgorithmElement *, const AlgorithmCommunication::PanaromaParameters, AlgorithmCommunication::ResponseOfRequests *response);
	GRPCSetReturn handleFaceDetection(AlgorithmElement *, const AlgorithmCommunication::FaceDetectionParameters, AlgorithmCommunication::ResponseOfRequests *response);
	int bufsize;
	QHash<Algorithm, QList<AlgorithmElement *> > algoElements;
	//Service Related Part
	GrpcThread *grpcServ;
	QFileSystemWatcher * watcher;
	//Driver Related Part
	PtzpHead *pt;
	TbgthDriver *tbgth;
	AryaDriver *arya;
	IRDomeDriver *botas;
	PtzpDriver *ptzp;
	System systemHandler;
	PTZinformation ptzInfo;
	AlarmInfo alarmInfo;
	KayiDriver *kayi;
	int setupDeviceController(const System systemInfo);
	void getPTZ();
	QTimer *timer;
	QList<AlgorithmCommunication::AlarmInfo> alarmQueue;
	QMutex alarmQueueLock;
};

#endif // ALGORITHMMANAGER_H
