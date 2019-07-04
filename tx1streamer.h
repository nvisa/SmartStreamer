#ifndef TX1STREAMER_H
#define TX1STREAMER_H

#include <lmm/players/basestreamer.h>
#include <algorithm/algorithmgrpcserver.h>

#include <QUrl>

class SeiInserter;
class RtpTransmitter;
class BaseRtspServer;
class BaseAlgorithmElement;
class alarmGeneratorElement;

class TX1Streamer : public BaseStreamer, public AlgoManIface
{
	Q_OBJECT
public:
	explicit TX1Streamer(QObject *parent = 0);

	virtual int start();
	BaseAlgorithmElement *getAlgo(int channel);
signals:

protected slots:
	void apiUrlRequested(const QUrl &url);

protected:
	int checkSeiAlarm(const RawBuffer &buf);
	int frameGenerator(const RawBuffer &buf);
	int notifyGrpcForAlarm(const RawBuffer &buf);
	int processBuffer(const RawBuffer &buf);

	virtual BaseLmmPipeline * createYUV420Pipeline(QSize &res0) = 0;

	SeiInserter *sei;
	BaseAlgorithmElement *privacy;
	BaseAlgorithmElement *motion;
	BaseAlgorithmElement *track;
	BaseAlgorithmElement *panchange;
	BaseLmmElement *enc0;
	BaseLmmElement *enc1;
	BaseLmmElement *enc2;
	BaseLmmElement *enc3;
	BaseLmmElement *textOverlay;
	AlgorithmGrpcServer *grpcserv;
	alarmGeneratorElement *algen;

	bool secondStream;
	bool thirdStream;
	bool fourthStream;
	bool enablePreview;

private:
	void finishGeneric420Pipeline(BaseLmmPipeline *p1, const QSize &res0);
};

#endif // TX1STREAMER_H
