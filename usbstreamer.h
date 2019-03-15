#ifndef USBSTREAMER_H
#define USBSTREAMER_H

#include "lmm/players/basestreamer.h"
#include "algorithmmanager.h"
class RtpTransmitter;
class SeiInserter;
class BaseRtspServer;

class UsbStreamer : public BaseStreamer
{
Q_OBJECT
public:
	explicit UsbStreamer(QObject *parent = 0);
	int generatePipelineForOneSource(const QString &sourceUrl);
	int PerformAlgorithmForYUV(const RawBuffer &buf);
	int setupAlgorithmManager();


protected:
	RtpTransmitter *rtpout;
	SeiInserter *sei;
	BaseRtspServer *rtspServer;
	AlgorithmManager *algMan;


	// BaseLmmElement interface
protected:
	int processBuffer(const RawBuffer &buf);
	int pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf);

};

#endif // USBSTREAMER_H
