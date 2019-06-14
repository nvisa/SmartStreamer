#include "panaromaalgorithmelement.h"

#include "lmm/debug.h"
#include "algorithmfunctions.h"

PanaromaAlgorithmElement::PanaromaAlgorithmElement(QObject *parent)
	: BaseAlgorithmElement(parent)
{
	algoState = UNKNOWN;
}

int PanaromaAlgorithmElement::init()
{
	v = BaseAlgorithmCommon::instance()->getAlgoParameters();
	return BaseAlgorithmElement::init();
}

int PanaromaAlgorithmElement::reallocate()
{
	control.pan = 0;
	control.tilt = 0;
	return 0;
}

int PanaromaAlgorithmElement::processAlgo(const RawBuffer &buf)
{
	int width = buf.constPars()->videoWidth;
	int height = buf.constPars()->videoHeight;

	float panTiltZoomRead[] = {0, 0, 0, 0, 0, 0};
	asel_pan((uchar *)buf.constData(), width * height, width, height, v.rgb,
			 v.shadow, v.ill, v.debug, control.meta, panTiltZoomRead, control.initialize);

	return newOutputBuffer(buf);
}

int PanaromaAlgorithmElement::baseAlgorithmProcess(const RawBuffer &buf)
{
	mInfo("Buffer sending with size %d", algoState);
	switch (algoState) {
	case INIT:
		init();
		algoState = REALLOCATE;
		break;
	case REALLOCATE:
		reallocate();
		algoState = PROCESS;
		break;
	case PROCESS:
		return processAlgo(buf);
		break;
	case STOPALGO:
		mDebug("Stopping Algorithm");
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

int PanaromaAlgorithmElement::release()
{
	asel_pan_release();
	return 0;
}

