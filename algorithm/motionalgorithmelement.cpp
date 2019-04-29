#include "motionalgorithmelement.h"

#include "lmm/debug.h"
#include "basealgorithmcommon.h"
#include "algorithmfunctions.h"

MotionAlgorithmElement::MotionAlgorithmElement(QObject *parent)
	: BaseAlgorithmElement(parent)
{
	algoState = UNKNOWN;
}

int MotionAlgorithmElement::init()
{
	v = BaseAlgorithmCommon::instance()->getAlgoParameters();
	control.initialize = 1;

	return BaseAlgorithmElement::init();
}

int MotionAlgorithmElement::reallocate()
{
	return 0;
}

int MotionAlgorithmElement::processAlgo(const RawBuffer &buf)
{
	QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);

	int width = buf.constPars()->videoWidth;
	int height = buf.constPars()->videoHeight;
	control.sensitivity = BaseAlgorithmCommon::instance()->getMotionSensitivity();
	control.classification = BaseAlgorithmCommon::instance()->getMotionClassification();

	asel_via_base((uchar*)buf.constData(), width * height, width, height,
				  v.rgb,v.shadow, v.ill, v.debug, v.stabilization, v.privacy,
				  control.meta, control.panTiltZoomRead, control.alarmFlag, control.initialize, control.sensitivity, control.classification, true);
	if (control.initialize)
		control.initialize = 0;

	QByteArray ba = QByteArray((char *)control.meta, 4096);
	hash.insert("motion_results", ba);
	((RawBuffer *)&buf)->pars()->metaData = RawBuffer::serializeMetadata(hash);
	return newOutputBuffer(buf);
}

int MotionAlgorithmElement::baseAlgorithmProcess(const RawBuffer &buf)
{
	mDebug("Process in %d", algoState);
	switch (algoState) {
	case INIT:
		init();
		algoState = PROCESS;
		break;
	case PROCESS:
		processAlgo(buf);
		break;
	case STOPALGO:
		stopAlgo();
		break;
	case RELEASE:
		release();
		break;
	default:
		break;
	}
	return newOutputBuffer(buf);
}

void MotionAlgorithmElement::setState(BaseAlgorithmElement::AlgoState state)
{
	algoState = state;
}
