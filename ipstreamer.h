#ifndef IPSTREAMER_H
#define IPSTREAMER_H

#include "tx1streamer.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

class RtspClient;
class RtpReceiver;
class BaseRtspServer;
class RtpTransmitter;
class FFmpegDecoder;
class SeiInserter;
class AlgorithmManager;


class IpStreamer : public TX1Streamer
{

Q_OBJECT

public:
	explicit IpStreamer(const QJsonObject &config, QObject *parent = 0);
	int generatePipelineForOneSource(const QString &Url);
	int generatePipelineForMultipleSource(const QStringList &Url);
	void addRtspServer(RtpTransmitter *rtpout, RtpTransmitter *rtpoutvs, int sno);
	int PerformAlgorithmForYUV(const RawBuffer &buf);
	int readSourceInformation();
	int setupAlgorithmManager();

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


	// BaseLmmElement interface
protected:
	int pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf) override;
	BaseLmmPipeline * createYUV420Pipeline(QSize &res0) override;
	void addExtraRtpTransmitters(QList<RtpTransmitter *> &list) override;

protected:
	RtpReceiver *rtp;
	RtspClient *rtsp;
	RtpTransmitter *rtpout;
	BaseRtspServer *rtspServer;
	FFmpegDecoder *dec;
	SeiInserter *sei;

	bool ipSourceEnabled;
	QString sourceAddress;
	int decBufferCount;
	int decWidth;
	int decHeight;
	int rtpBufferDuration = 0;
	float decOutputInFps;
	float decOutputOutFps;
	QHash <int,int> enc_bitrate_set;
	QJsonObject config;

	QString rtspClientUser;
	QString rtspClientPass;
	AlgorithmManager *algMan;

	friend class MultiSource;
};

#endif // IPSTREAMER_H
