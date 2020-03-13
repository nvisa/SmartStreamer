#ifndef MOXAK1STREAMER_H
#define MOXAK1STREAMER_H

#include <lmm/players/basestreamer.h>
#include "algorithm/algorithmgrpcserver.h"

class RtpReceiver;
class TK1OmxPipeline;
class BaseAlgorithmElement;

class MoxaK1Streamer : public BaseStreamer, public AlgoManIface
{
	Q_OBJECT

class SourceInformation
{
public:
	QString sourceIpAddr;
	int width;
	int height;
	float fps;

	QString rtspId;
	QString rtspPass;
};

public:
	explicit MoxaK1Streamer(const QJsonObject &config, QObject *parent = 0);
	int generatePipeline(const QString &url);
	int runAlgorithm(int channel);
	int stopAlgorithm(int channel);
	int setPixelFormat(const RawBuffer &buf);

	// PipelineManager interface
protected:
	int pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf);

private:
	RtpReceiver *rtp;
	TK1OmxPipeline *dec;
	TK1OmxPipeline *enc;
	BaseAlgorithmElement *motion;
	BaseAlgorithmElement *stabilization;
	BaseAlgorithmElement *track;
	AlgorithmGrpcServer *grpcserv;

	SourceInformation *sourceInfo;
};

#endif // MOXAK1STREAMER_H
