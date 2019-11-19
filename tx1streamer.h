#ifndef TX1STREAMER_H
#define TX1STREAMER_H

#include "alarmsource.h"

#include <lmm/players/basestreamer.h>
#include <algorithm/algorithmgrpcserver.h>

#include <QUrl>
#include <QSharedPointer>

class VideoScaler;
class SeiInserter;
class RtpTransmitter;
class BaseRtspServer;
class BaseAlgorithmElement;
class alarmGeneratorElement;
class InternalRecorder;
class BufferQueue;

class TX1Streamer : public BaseStreamer, public AlgoManIface
{
	Q_OBJECT
public:
	explicit TX1Streamer(QObject *parent = 0);

	virtual int start();

	int runAlgorithm(int channel);
	int stopAlgorithm(int channel);
	~TX1Streamer();
signals:

protected slots:
	void apiUrlRequested(const QUrl &url);

protected:
	int checkSeiAlarm(const RawBuffer &buf);
	int frameGenerator(const RawBuffer &buf);
	int notifyGrpcForAlarm(const RawBuffer &buf);
	int notifyGrpcForMotionAlarm(const RawBuffer &buf);
	int processBuffer(const RawBuffer &buf);
	int recordIfNvrDead(const RawBuffer &buf);
	void enableRGBPortion(bool en);

	virtual void addExtraRtpTransmitters(QList<RtpTransmitter *> &) {}
	virtual BaseLmmPipeline * createYUV420Pipeline(QSize &res0) = 0;

	SeiInserter *sei;
	BaseAlgorithmElement *privacy;
	BaseAlgorithmElement *motion;
	BaseAlgorithmElement *track;
	BaseAlgorithmElement *panchange;
	BaseAlgorithmElement *face;
	BaseLmmElement *enc0;
	BaseLmmElement *enc1;
	BaseLmmElement *enc2;
	BaseLmmElement *enc3;
	BaseLmmElement *textOverlay;
	AlgorithmGrpcServer *grpcserv;
	alarmGeneratorElement *algen;
	InternalRecorder* recorder;
	VideoScaler *yuv2rgb;
	VideoScaler *rgb2yuv;
	BufferQueue *jpegQueue;
	QSharedPointer<MotionAlarmSource> motionAlarmSource;
	QSharedPointer<TrackAlarmSource> trackAlarmSource;
	QSharedPointer<GenericAlarmSource> tamperAlarmSource;

	bool secondStream;
	bool thirdStream;
	bool fourthStream;
	bool enablePreview;
	bool videoRecordingEnabled;

private:
	enum AlgorithmState {
		NONE,
		TO_GPU_FREE,
		GPU_FREE,
		TO_PRIVACY_RUNNING,
		PRIVACY_RUNNING,
		TO_MOTION_RUNNING,
		MOTION_RUNNING,
		TO_TRACK_RUNNING,
		TRACK_RUNNING,
		TO_DIFF_RUNNING,
		DIFF_RUNNING,
		TO_FACE_RUNNING,
		FACE_RUNNING,
	};

	AlgorithmState algos;
	AlgorithmState algosPending;
	bool motionExtraEnabled;

	void checkAlgoState();
	void finishGeneric420Pipeline(BaseLmmPipeline *p1, const QSize &res0);
};

#endif // TX1STREAMER_H
