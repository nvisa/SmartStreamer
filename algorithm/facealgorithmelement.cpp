#include "facealgorithmelement.h"

#include <lmm/debug.h>

#include "algorithmfunctions.h"

FaceAlgorithmElement::FaceAlgorithmElement(QObject *parent)
	: BaseAlgorithmElement(parent)
{

}

int FaceAlgorithmElement::init()
{
	v.rgb  =1;
	control.initialize = 1;
	control.camid = 0;
	control.frameRate = 25;
	control.counter = 0;
	return 0;
}

int FaceAlgorithmElement::reallocate()
{

}

int FaceAlgorithmElement::processAlgo(const RawBuffer &buf)
{
	control.counter++;
	int div = 100 / control.frameRate;
	if (control.counter != div)
		return newOutputBuffer(buf);
	control.counter = 0;
	v.debug = 1;
	int numFace = 0;
	asel_face((uchar *)buf.constData(), numFace, v.debug, control.meta, control.initialize, control.camid);
	if (control.initialize)
		control.initialize = 0;
	control.numface = numFace;
	return newOutputBuffer(buf);
}

int FaceAlgorithmElement::baseAlgorithmProcess(const RawBuffer &buf)
{
	switch (algoState) {
	case INIT:
		init();
		algoState = PROCESS;
		break;
	case PROCESS:
		return processAlgo(buf);
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

int FaceAlgorithmElement::release()
{
	asel_face_release();
	return 0;
}

