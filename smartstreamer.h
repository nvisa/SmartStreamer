#ifndef SMARTSTREAMER_H
#define SMARTSTREAMER_H

#include <lmm/players/basestreamer.h>

#include "proto/config.grpc.pb.h"

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
class SmartStreamer : public BaseStreamer, public config::AppConfig::Service
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

    grpc::Status SetCurrentMode(grpc::ServerContext *context, const config::SetModeQ *request, config::AppCommandResult *response);
    grpc::Status GetCurrentMode(grpc::ServerContext *context, const config::DummyInfo *request, config::AppCommandResult *response);
    grpc::Status SetPanaromaParameters(grpc::ServerContext *context, const config::PanoramaPars *request, config::AppCommandResult *response);
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
    CudaConfigurations *cuConf;
    GrpcThread *grpcServ;
    GrpcPTZClient *ptzclient;
};

#endif // SMARTSTREAMER_H
