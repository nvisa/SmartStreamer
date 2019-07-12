#ifndef FLIRSTREAMER_H
#define FLIRSTREAMER_H

#include <lmm/players/basestreamer.h>
#include "algorithm/algorithmgrpcserver.h"
#include <QElapsedTimer>

class RtpReceiver;
class SeiInserter;
class TK1OmxPipeline;
class FlirStreamerPriv;
class BaseAlgorithmElement;

class FlirStreamer: public BaseStreamer
{
	Q_OBJECT
public:
	explicit FlirStreamer(const QJsonObject &config, QObject *parent = 0);
	int generatePipeline(const QString &url);
protected:
	int pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf);
	int PerformSEI(const RawBuffer &buf);
private:
	RtpReceiver *rtp;
	SeiInserter *sei;
	TK1OmxPipeline *dec;
	FlirStreamerPriv *priv;
	BaseAlgorithmElement *motion;
	AlgorithmGrpcServer *grpcserv;
};

#endif // FLIRSTREAMER_H
