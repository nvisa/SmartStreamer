#ifndef USBSTREAMER_H
#define USBSTREAMER_H

#include "lmm/players/basestreamer.h"
#include "algorithm/algorithmgrpcserver.h"
#include "algorithm/motionalgorithmelement.h"

#include <ecl/ptzp/irdomedriver.h>

#include <QUrl>

class RtpTransmitter;
class SeiInserter;
class BaseRtspServer;
class UsbStreamer : public BaseStreamer
{
Q_OBJECT
public:
	explicit UsbStreamer(QObject *parent = 0);
	int generatePipelineForOneSource();
	int checkSeiAlarm(const RawBuffer &buf);

protected slots:
	void apiUrlRequested(const QUrl &url);

protected:
	int processBuffer(const RawBuffer &buf);
	int pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf);

	SeiInserter *sei;
	BaseAlgorithmElement *privacy;
	BaseAlgorithmElement *motion;
	BaseAlgorithmElement *track;
	AlgorithmGrpcServer *grpcserv;
};

#endif // USBSTREAMER_H
