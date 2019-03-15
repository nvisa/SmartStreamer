#ifndef ALGORITHMMANAGER_H
#define ALGORITHMMANAGER_H


#include "lmm/baselmmelement.h"

#include "ecl/ptzp/ptzphead.h"
#include "proto/AlgorithmCommunication.grpc.pb.h"

#include "ecl/debug.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include <vector>
#include <iostream>
using namespace std;

class TbgthDriver;
class AryaDriver;
class IRDomeDriver;

class RawBuffer;
class PtzpHead;
class PtzpDriver;
class AlgorithmElement;

class AlgorithmManager : public AlgorithmCommunication::AlgorithmService::Service
{
	//Q_OBJECT
public:
	explicit AlgorithmManager(QObject *parent);
	~AlgorithmManager();
	int openAlgRelatedJson();
	int setupDeviceController(const QString &deviceInfo);
	bool checkSystemParameters();

	void startGrpc();


	//GRPC related functions
	grpc::Status RunAlgorithm(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response);
	grpc::Status StopAlgorithm(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response);
	grpc::Status SetAlgorithmParameters(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfRequests *response);
	grpc::Status GetAlgorithmParameters(grpc::ServerContext *context, const AlgorithmCommunication::RequestForAlgorithm *request, AlgorithmCommunication::ResponseOfParameterRequests *response);
	grpc::Status SetSystemFeatures(grpc::ServerContext *context, const AlgorithmCommunication::RequestForSystemFeatures *request, AlgorithmCommunication::ResponseOfSystemFeatures *response);
	grpc::Status GetSystemFeatures(grpc::ServerContext *context, const AlgorithmCommunication::RequestForSystemFeatures *request, AlgorithmCommunication::ResponseOfSystemFeatures *response);

	//grpc::Status SetCurrentMode(grpc::ServerContext *context, const OrionCommunication::ModeQ *request, OrionCommunication::AppCommandResult *response);


	enum AlgorithmState {
		NOT_INIT = 0,
		INIT,
		RE_INIT,
		ON_THE_RUN,
		IDLE,
		HOLD,
		TERMINATED,
		WOW,
	};

	enum Algorithm {
		MOTION,
		STABILIZATION,
		TRACKING,
		PANAROMA,
		FACE_DETECTION,
		NONE,
	};

	enum TrackMode {
		AUTO,
		SEMI_AUTO,
		MANUAL,
	};

	enum System {
		BOTAS,
		TBGTH,
		ARYA,
		TEST,
		EMPTY_SYSTEM,
	};

	enum PTZactions {
		PAN_STOP,
		PAN_RIGHT,
		PAN_LEFT,
		TILT_UP,
		TILT_DOWN,
		TILT_STOP,
		PAN_RIGHT_TILT_UP,
		PAN_RIGHT_TILT_DOWN,
		PAN_LEFT_TILT_UP,
		PAN_LEFT_TILT_DOWN,
		PAN_TILT_POS,
	};

	struct PTZinformation {
		PTZactions action;
		float pan;
		float tilt;
		float zoom;
	};

	struct deviceProperties {
        QString cameraIp;
        int width;
		int height;
		int fps;
		int frameSize;
	};

	struct parameters {
		int rgb;
		int ill;
		int debug;
		int shadow;
		int record;
		int privacy;
		int stabilization;
	};

	struct configurationUnit {
		parameters param;
		deviceProperties devProp;
	};

	struct MotionAlg {
		int sensitivity;
        bool classification_;
		bool alarmFlag;
        int classification;
	};

	struct Stabilization {
		int sensitivity;
		int dummy;
	};

	struct TrackedObjects {
		float objWidth;
		float objHeight;
		float objPointX;
		float objPointY;
	};

	struct Tracking {
		float trackScore;
		int trackDuration;
		QMap<int,TrackMode> enabledModes;
		QMap<int, TrackedObjects> objects;
		float objWidth;
		float objHeight;
		float objPointX;
		float objPointY;
		int dummy;
	};

	struct Panaroma {
		int dummy;
	};

	struct FaceDetection {
		int dummy;
        bool isTileOn;
        int xTile;
        int yTile;
        int mode; // 0:rectangle, 1:privacy, else:imagecroplist
        bool isAlignmentOn;
    };

	struct PanTiltZoomInfo {
		float pan;
		float tilt;
		float zoom;
	};

	struct AlgorithmHandler {
		MotionAlg motionA;
		Stabilization stabilA;
		Tracking trackA;
		Panaroma panaromA;
		FaceDetection faceA;
		AlgorithmState stateOfAlg;
		Algorithm currentActiveAlg;
		int initialize;
		uchar meta[4096];
		System systemHandler;
		PanTiltZoomInfo locationInfo;
		bool systemParameters;
		configurationUnit confUnit;
		PTZinformation infoAboutPTZ;
	};
	AlgorithmHandler algHandler;

	AlgorithmHandler getAlgHandlerFor(int index);
	void registerAlgorithm(Algorithm alg, AlgorithmElement *el);
	// BaseLmmElement interface
protected:

	// PtzpHead interface
public:
    QMap<Algorithm,bool> availableAlgortihms;
protected:
	//Algorithm state related functions


	//System&buffer related params
	configurationUnit confUnit;
	QMap<int,Algorithm> availableAlgList;
	int bufsize;
	QHash<Algorithm, QList<AlgorithmElement *> > algoElements;
	//Fov okuma ve interpolasyonla ilgili fonksiyonlar da eklenecek
	//GRPCleri buraya taşımamız mantıklı olacak

	//Algorithm manager instance

	//algorithm helpers
	void RunMotion(const MotionAlg &mParams);
	void RunStab();


	float pan_tilt_zoom_read[];

	//System Drivers and functions
	QString deviceInfo;
	PtzpHead *pt;
	TbgthDriver *tbgth;
	AryaDriver *arya;
	IRDomeDriver *botas;
	PtzpDriver *ptzp;
	int setPT(PTZinformation info);
	int setZoom(uint pos);
	void getPTZ();

};

#endif // ALGORITHMMANAGER_H
