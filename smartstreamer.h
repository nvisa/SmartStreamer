#ifndef SMARTSTREAMER_H
#define SMARTSTREAMER_H

#include "lmm/players/basestreamer.h"
#include "proto/OrionCommunication.grpc.pb.h"

#include <QJsonObject>

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
class IRDomeDriver;
class PtzpDriver;
class TbgthDriver;
class TX1VideoEncoder;
class TbgthData;
class GpioController;

class SmartStreamer : public BaseStreamer, public OrionCommunication::OrionCommunicationService::Service
{
	Q_OBJECT
public:
	explicit SmartStreamer(QObject *parent = 0);

	void reboot(int seconds);
	int setupTbgthCombined(const QString &rtspUrl1, const QString &rtspUrl2);
	int setupRtspClient(const QString &rtspUrl);
	void setupVideoAnalysis();
	void setupPanTiltZoomDriver(const QString &target);

	int processMainYUV(const RawBuffer &buf);
	int processMainYUVThermal(const RawBuffer &buf);
	int processMainRGB(const RawBuffer &buf);
	int processScaledRGB(const RawBuffer &buf);
	int processScaledYUV(const RawBuffer &buf);
	int checkPoint(const RawBuffer &buf);

	void ptzCommandRecved(int cmd);

	grpc::Status SetCurrentMode(grpc::ServerContext *context, const OrionCommunication::ModeQ *request, OrionCommunication::AppCommandResult *response);
	grpc::Status GetCurrentMode(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::ModeQ *response);
	grpc::Status SetVideoOverlay(grpc::ServerContext *context, const OrionCommunication::OverlayQ *request, OrionCommunication::AppCommandResult *response);
	grpc::Status SetPanaromaParameters(grpc::ServerContext *context, const OrionCommunication::PanoramaPars *request, OrionCommunication::AppCommandResult *response);
	grpc::Status GetPanaromaFrames(grpc::ServerContext *context, const OrionCommunication::FrameModeQ *request, ::grpc::ServerWriter<OrionCommunication::PanoramaFrame> *writer);
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
	grpc::Status RunCalibration(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response);
	grpc::Status StopCalibration(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response);
	grpc::Status RunAutoTrack(grpc::ServerContext *context, const OrionCommunication::AutoTrackQ *request, OrionCommunication::AppCommandResult *response);
	grpc::Status StopAutoTrack(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response);
	grpc::Status RunStabilization(grpc::ServerContext *context, const OrionCommunication::DevicedBasedInfo *request, OrionCommunication::AppCommandResult *response);
	grpc::Status StopStabilization(grpc::ServerContext *context, const OrionCommunication::DevicedBasedInfo *request, OrionCommunication::AppCommandResult *response);
	grpc::Status GetUptime(grpc::ServerContext *context, const OrionCommunication::DevicedBasedInfo *request, OrionCommunication::ReturnUptimeValue *response);
	grpc::Status GetFovValue(grpc::ServerContext *context, const OrionCommunication::DevicedBasedInfo *request, OrionCommunication::ReturnFOVValue *response);
	grpc::Status SetItemProperty(grpc::ServerContext *context, const OrionCommunication::ItemProperty *request, OrionCommunication::AppCommandResult *response);
	grpc::Status GetItemProperty(grpc::ServerContext *context, const OrionCommunication::ItemProperty *request, OrionCommunication::ItemProperty *response);
	grpc::Status SetBitrate(grpc::ServerContext *context, const OrionCommunication::BitrateValue *request, OrionCommunication::AppCommandResult *response);
	grpc::Status GetBitrate(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::BitrateValue *response);
	grpc::Status RunCIT(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, ::grpc::ServerWriter<OrionCommunication::CitMessage> *writer);

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
			ptzUrl = "eth;10.5.20.92:8998";
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
		QString ptzUrl;
		QString rtspUrl;
	};
	Parameters pars;
	Parameters pars2;
	QJsonObject sets;

	QByteArray doScreenShot(const RawBuffer &buf);
signals:
	void rebootMe(int seconds);
public slots:
	void rebootSlot(int seconds);
	virtual void timeout();
protected:
	void addRtspServer(RtpTransmitter *rtpout, RtpTransmitter *rtpoutvs, int sno);
	void printParameters();
	int pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf);

	RtpReceiver *rtp;
	RtspClient *rtsp;
	RtpTransmitter *rtpout;
	BaseRtspServer *rtspServer;
	ViaWrapper *wrap;
	ViaWrapper *wrapThermal;
	ViaWrapper *wrapDayTv;
	FFmpegDecoder *dec;
	QtVideoOutput *vout;
	SeiInserter *sei;
	GrpcThread *grpcServ;
	RawBuffer screenBuf;
	RawBuffer screenSecBuf;
	TbgthDriver *ptzp;
	PtzpHead *pt;
	PtzpHead *thermalCam;
	int width;
	int height;
	int period;
	bool enableVideoStabilization;
	bool enableVideoTrack;
	bool goToZeroPosition();
	bool startSpinnig(float sSpeed = 0);
	void doPanaroma(const RawBuffer &buf);
	void doCalibration(const RawBuffer &buf);
	void doMotionDetection(const RawBuffer &buf);
	void doDirectTrack(const RawBuffer &buf, ViaWrapper *wrap);
	void doStabilization(const RawBuffer &buf, ViaWrapper *wrap);
	QByteArray getImageFromFile(const QString &filename);
	QByteArray convertImageToByteArray(const QString &filename);
	QMutex mutex;
	QMutex viaLock;

	bool ptzpStatus;
	bool getScreenShot;
	bool triggeredByTestAPI;
	int setPtz(uchar meta[], ViaWrapper *wrap);
	float previousPanValue;
	float previousTiltValue;
	float panValue;
	float tiltValue;
	int counterForStabil;

	QElapsedTimer dayPipelineElapsed;
	QElapsedTimer thPipelineElapsed;
	GpioController *gpiocont;

	friend class TbgthData;
};

#endif // SMARTSTREAMER_H
