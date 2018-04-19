#ifndef SMARTSTREAMER_H
#define SMARTSTREAMER_H

#include <lmm/players/basestreamer.h>

#include "proto/OrionCommunication.grpc.pb.h"

#include <QTcpSocket>

class RtspClient;
class RtpReceiver;
class BaseRtspServer;
class RtpTransmitter;
class ViaWrapper;
class FFmpegDecoder;
class QtVideoOutput;
class SeiInserter;
class CudaConfigurations;
class GrpcThread;
class GrpcPTZClient;
class SmartStreamer : public BaseStreamer, public OrionCommunication::AppConfig::Service
{
	Q_OBJECT
public:
	explicit SmartStreamer(QObject *parent = 0);

	int setupRtspClient(const QString &rtspUrl);

	int processMainYUV(const RawBuffer &buf);
	int processMainRGB(const RawBuffer &buf);
	int processScaledRGB(const RawBuffer &buf);
	int processScaledYUV(const RawBuffer &buf);
    int processPanoramaImage(const RawBuffer &buf);
	int checkPoint(const RawBuffer &buf);

    grpc::Status SetCurrentMode(grpc::ServerContext *context, const OrionCommunication::SetModeQ *request, OrionCommunication::AppCommandResult *response);
    grpc::Status GetCurrentMode(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::SetModeQ *response);
    grpc::Status SetPanaromaParameters(grpc::ServerContext *context, const OrionCommunication::PanoramaPars *request, OrionCommunication::AppCommandResult *response);
    grpc::Status GetPanaromaFrames(grpc::ServerContext *context, const OrionCommunication::GetFrames *request, ::grpc::ServerWriter<OrionCommunication::PanoramaFrame> *writer);
    grpc::Status RunPanaroma(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response);
    grpc::Status RunMotion(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response);
    grpc::Status StopPanaroma(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response);
    grpc::Status StopMotion(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response);
    grpc::Status GetScreenShot(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::ScreenFrame *response);
    grpc::Status SetMotionDetectionParameters(grpc::ServerContext *context, const OrionCommunication::TRoi *request, OrionCommunication::AppCommandResult *response);
	class Parameters {
	public:
		Parameters()
		{
			decWidth = 0;
			decHeight = 0;
			pipelineFlags = 0xffffffff;
			secWidth = 0;
			secHeight = 0;
			rgbMainWidth = 640;
			rgbMainHeight = 360;
			decBufferCount = 60;
			rtpBufferDuration = 0;
			enableMoxaHacks = false;
			decOutputInFps = 0;
            decOutputOutFps = 0;
		}

		enum EnabledElemenets {
			EL_YUV_PROCESS = 1 << 0,
			EL_YUVS_PROCESS = 1 << 1,
			EL_RGB_PROCESS = 1 << 2,
			EL_RGBS_PROCESS = 1 << 3,
			EL_RTP_OUTPUT = 1 << 4,
			EL_QT_VOUT = 1 << 5,
			EL_MJPEG_OUTPUT = 1 << 6,
		};

		bool enableMoxaHacks;
		int decBufferCount;
		int decWidth;
		int decHeight;
		int secWidth;
		int secHeight;
		int rgbMainWidth;
		int rgbMainHeight;
		uint pipelineFlags;
		int rtpBufferDuration;
		QString rtspClientUser;
		QString rtspClientPass;
		QString rtspServerUser;
		QString rtspServerPass;
		float decOutputInFps;
		float decOutputOutFps;
	};
	Parameters pars;

    int processCudaMode(const RawBuffer &buf);
    int processCudaBaseMode(const RawBuffer &buf);
    QString doPtzCommand(QString ascii);
    void initPanaroma();
    void GoToPositionPanaroma();
signals:

public slots:
    void connected();
    void switchEvent();
	virtual void timeout();
protected:
	void printParameters();
	int pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf);

	RtpReceiver *rtp;
	RtspClient *rtsp;
	RtpTransmitter *rtpout;
	BaseRtspServer *rtspServer;
	ViaWrapper *wrap;
	FFmpegDecoder *dec;
	QtVideoOutput *vout;
	SeiInserter *sei;
    CudaConfigurations *cuConf;
    GrpcThread *grpcServ;
    GrpcPTZClient *ptzclient;
    unsigned char *screenBuffer;
    int width;
    int height;

    QTimer *timer;
    QTcpSocket *socket;


    //Pan speed
    QString speed;
    //Pan&Tilt values
    QString panVal;
    QString tiltVal;

    //flag for switchevent slot
    bool panInitOnce;
    bool baseInitOnce;

    //flags for algorithms
    bool panaroma;
    bool base;

    //flags for init of algorithms
    bool initAlgoritmOnce;
    bool initBaseAlgoritmOnce;
    int init;
    int initPanInViaBa;
    int initBaseInViaBa;
    //pan counter to set init location before going to next step
    int panaromaCounter;

    //pan_states_init
    bool panInit;
    bool panMotionStart;
    bool panTiltInfo;
    bool panStop;
    //pan_states_end
    bool baseStop;
    QElapsedTimer timerElapsed;
    qint64 prevTime;
};

#endif // SMARTSTREAMER_H
