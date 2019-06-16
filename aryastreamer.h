#ifndef ARYASTREAMER_H
#define ARYASTREAMER_H

#include "lmm/players/basestreamer.h"

class RtpReceiver;
class RtspClient;
class FFmpegDecoder;
class BaseAlgorithmElement;
class SeiInserter;
class AlgorithmGrpcServer;
class AryaStreamerPriv;
class SnapshotElement;
class OrionCommunicationServer;
class AryaStreamer: public BaseStreamer
{
	Q_OBJECT
public:
	explicit AryaStreamer(const QJsonObject &config, QObject *parent = 0);
	int generatePipeline(const QString &url);
	int PerformSEI(const RawBuffer &buf);

protected:
	int pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf);
	int createAlgorithms();
private:
	SeiInserter *sei;
	RtpReceiver *rtp;

	BaseAlgorithmElement *motion;
	BaseAlgorithmElement *panaroma;
	AlgorithmGrpcServer *grpcserv;
	OrionCommunicationServer *grpcorion;
	AryaStreamerPriv *priv;
	SnapshotElement *snap;
};

#endif // ARYASTREAMER_H
