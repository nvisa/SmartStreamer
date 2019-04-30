#include "stabilizationalgorithmelement.h"

#include "algorithmfunctions.h"
#include <lmm/debug.h>

StabilizationAlgorithmElement::StabilizationAlgorithmElement(QObject *parent)
	: BaseAlgorithmElement(parent)
{

}

int StabilizationAlgorithmElement::init()
{
	v = BaseAlgorithmCommon::instance()->getAlgoParameters();
	v.stabilization = 1;
	control.initialize = 1;
	return BaseAlgorithmElement::init();
}

int StabilizationAlgorithmElement::reallocate()
{
	return 0;
}

int StabilizationAlgorithmElement::processAlgo(const RawBuffer &buf)
{
	int width = buf.constPars()->videoWidth;
	int height = buf.constPars()->videoHeight;
	control.sensitivity = BaseAlgorithmCommon::instance()->getSensitivity("stabilization");
	asel_bypass((uchar *)buf.constData(), width * height, width, height,
				v.debug, v.stabilization,v.privacy,
				control.initialize, control.sensitivity, v.rgb);
	if (control.initialize)
		control.initialize = 0;

	mInfo("Stabilization algorithm processing");
	return newOutputBuffer(buf);
}

int StabilizationAlgorithmElement::baseAlgorithmProcess(const RawBuffer &buf)
{
	mInfo("Buffer sending with size %d", algoState);
	switch (algoState) {
	case INIT:
		init();
		algoState = PROCESS;
		break;
	case PROCESS:
		return processAlgo(buf);
		break;
	case RELEASE:
		release();
		break;
	default:
		break;
	}
	return newOutputBuffer(buf);
}

int StabilizationAlgorithmElement::release()
{
	asel_bypass_release();
	return 0;
}

