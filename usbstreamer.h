#ifndef USBSTREAMER_H
#define USBSTREAMER_H

#include "lmm/players/basestreamer.h"
#include "algorithm/algorithmgrpcserver.h"
#include "algorithm/motionalgorithmelement.h"

#include <ecl/ptzp/irdomedriver.h>

class RtpTransmitter;
class SeiInserter;
class BaseRtspServer;
class UsbStreamer : public BaseStreamer
{
Q_OBJECT
public:
	explicit UsbStreamer(QObject *parent = 0);
	int generatePipelineForOneSource();
	int PerformAlgorithmForYUV(const RawBuffer &buf);
	int startPt(const QString &address);

protected:
	SeiInserter *sei;
	// BaseLmmElement interface
protected:
	int processBuffer(const RawBuffer &buf);
	int pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf);

	MotionAlgorithmElement *motion;
	AlgorithmGrpcServer *grpcserv;
	IRDomeDriver *botas;
};

#endif // USBSTREAMER_H
