#ifndef TK1STREAMER_H
#define TK1STREAMER_H

#include <lmm/players/basestreamer.h>
#include "algorithm/algorithmgrpcserver.h"

class TK1StreamerPriv
{
public:
	float fps;
	int width;
	int height;
	QString url;
	QString rtspUser;
	QString rtspPass;
	bool orionComm;
	QString receiverType;
	QString decoderType;
	QString rtspTransport;
};

class SeiInserter;
class alarmGeneratorElement;
class RtspClient;
class RtpReceiver;
class BufferQueue;
class TK1Streamer: public BaseStreamer, public AlgoManIface
{
	Q_OBJECT
public:
	explicit TK1Streamer(const QJsonObject &obj, QObject *parent = 0);
	virtual int start();
	int runAlgorithm(int channel);
	int stopAlgorithm(int channel);
	~TK1Streamer();
protected:
	virtual BaseLmmPipeline *generatePipeline() = 0;
	int PerformSEI(const RawBuffer &buf);
	void finishPipeline(BaseLmmPipeline *p1);
	int processBuffer(const RawBuffer &buf);
	void createDecoderEl(QString type);
	void createReceiverEl(QString type);
protected:
	RtspClient *rtsp;
	BufferQueue *queue;
	BaseLmmElement *rtp;
	BaseLmmElement *decoder;
	alarmGeneratorElement *algen;
private:
	SeiInserter *sei;
	TK1StreamerPriv *priv;
	BaseAlgorithmElement *motion;
	BaseAlgorithmElement *panaroma;
};

#endif // TK1STREAMER_H
