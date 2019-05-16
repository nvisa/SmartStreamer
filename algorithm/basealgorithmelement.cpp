#include "basealgorithmelement.h"

#include "lmm/debug.h"

BaseAlgorithmElement::BaseAlgorithmElement(QObject *parent)
	: BaseLmmElement(parent)
{
	algoState = UNKNOWN;
}

int BaseAlgorithmElement::init()
{
	return 0;
}

int BaseAlgorithmElement::gpuInit()
{
	return 0;
}

int BaseAlgorithmElement::reinit()
{
	return 0;
}

int BaseAlgorithmElement::processAlgo(const RawBuffer &buf)
{
	Q_UNUSED(buf);
	return 0;
}

/* This flow is base algorithm process if you want to use this flow you can implement on your application.
 * Watch out: After implementation, used buffer send to after element at the pipe. You must use newOutputBuffer function
 *
*/

int BaseAlgorithmElement::baseAlgorithmProcess(const RawBuffer &buf)
{
	mInfo("Buffer sending with size %d", algoState);
	switch (algoState) {
	case INIT:
		init();
		algoState = GPUINIT;
		break;
	case GPUINIT:
		gpuInit();
		algoState = REINIT;
		break;
	case REINIT:
		reinit();
		algoState = PROCESS;
		break;
	case PROCESS:
		processAlgo(buf);
		break;
	case REALLOCATE:
		algoState = STOPALGO;
		reallocate();
		break;
	case STOPALGO:
		algoState = RELEASE;
		stopAlgo();
		break;
	case RELEASE:
		algoState = UNKNOWN;
		release();
		break;
	default:
		break;
	}

	return newOutputBuffer(buf);
}

int BaseAlgorithmElement::reallocate()
{
	return 0;
}

int BaseAlgorithmElement::stopAlgo()
{
	return 0;
}

int BaseAlgorithmElement::release()
{
	return 0;
}

int BaseAlgorithmElement::processBuffer(const RawBuffer &buf)
{
	return baseAlgorithmProcess(buf);
}

