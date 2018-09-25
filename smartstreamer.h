#ifndef SMARTSTREAMER_H
#define SMARTSTREAMER_H

#include "lmm/players/basestreamer.h"
#include "proto/OrionCommunication.grpc.pb.h"

class RtspClient;
class RtpReceiver;
class BaseRtspServer;
class RtpTransmitter;
class ViaWrapper;
class FFmpegDecoder;
class QtVideoOutput;
class SeiInserter;
class GrpcThread;
class PtzpHead;
class AryaDriver;
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
    grpc::Status GetSecScreenShot(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::ScreenFrame *response);
    grpc::Status GetMainScreenShot(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::ScreenFrame *response);
    grpc::Status SetMotionDetectionParameters(grpc::ServerContext *context, const OrionCommunication::TRoi *request, OrionCommunication::AppCommandResult *response);
    grpc::Status GetMotionDetectionParameters(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::TRoi *response);
    grpc::Status GetSensivityParameter(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::SetSensivity *response);
    grpc::Status SetSensivityParameter(grpc::ServerContext *context, const OrionCommunication::SetSensivity *request, OrionCommunication::AppCommandResult *response);
    grpc::Status GotoPanaromaPixel(grpc::ServerContext *context, const OrionCommunication::TPoint *request, OrionCommunication::AppCommandResult *response);
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

    QByteArray doScreenShot(const RawBuffer &buf);
signals:

public slots:
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
	GrpcThread *grpcServ;
	QByteArray screenMainShot;
	QByteArray screenSecShot;
	AryaDriver *arya;
	PtzpHead *pt;
	PtzpHead *thermalCam;
	int width;
	int height;

	bool goToZeroPosition();
	bool startSpinnig(float sSpeed = 0);
	void doPanaroma(const RawBuffer &buf);
	void doMotionDetection(const RawBuffer &buf);
	QByteArray getImageFromFile(const QString &filename);
	QByteArray convertImageToByteArray(const QString &filename);
	QMutex mutex;
};

#endif // SMARTSTREAMER_H
