#ifndef TK1STREAMER_H
#define TK1STREAMER_H

#include <lmm/players/basestreamer.h>
#include <algorithm/algorithmgrpcserver.h>
#include <orioncommunicationserver.h>
#include <lmm/rtp/rtpreceiver.h>
#include <lmm/rtsp/rtspclient.h>
#include <lmm/ffmpeg/baselmmdemux.h>

#include <lmm/bufferqueue.h>

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
};

class SeiInserter;
class alarmGeneratorElement;
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
	TK1StreamerPriv *priv;
	BaseAlgorithmElement *motion;
	BaseAlgorithmElement *panaroma;
	RtpReceiver *rtp;
	SeiInserter *sei;
	BufferQueue *queue;
	alarmGeneratorElement *algen;
	BaseLmmDemux *rtpmux;
	RtspClient *rtsp;

	void createReceiverEl(QString type);
};

#endif // TK1STREAMER_H
