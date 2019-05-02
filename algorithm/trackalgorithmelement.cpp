#include "trackalgorithmelement.h"

#include "lmm/debug.h"
#include "algorithmfunctions.h"

TrackAlgorithmElement::TrackAlgorithmElement(QObject *parent)
	: BaseAlgorithmElement(parent)
{

}

int TrackAlgorithmElement::init()
{
	v = BaseAlgorithmCommon::instance()->getAlgoParameters();
	control.initialize = 1;
	control.obj.point_x = 100;
	control.obj.point_y = 100;
	control.obj.width = 100;
	control.obj.height = 100;
	return BaseAlgorithmElement::init();
}

int TrackAlgorithmElement::reallocate()
{
	objProp[0] = control.obj.point_x;
	objProp[1] = control.obj.point_y;
	objProp[2] = control.obj.width;
	objProp[3] = control.obj.height;
	return BaseAlgorithmElement::reallocate();
}

int TrackAlgorithmElement::processAlgo(const RawBuffer &buf)
{
	BaseAlgorithmCommon *b = BaseAlgorithmCommon::instance();
	if (b->isTrackingAuto())
		return autoTrack(buf);
	else if (b->isTrackingSemiAuto())
		return semiAutoTrack(buf);
	else if (b->isTrackingManual())
		return manualTrack(buf);
	mDebug("No selected mode");
	return newOutputBuffer(buf);
}

int TrackAlgorithmElement::baseAlgorithmProcess(const RawBuffer &buf)
{
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

int TrackAlgorithmElement::release()
{
	asel_direct_track_release();
	return 0;
}

int TrackAlgorithmElement::autoTrack(const RawBuffer &buf)
{
	int width = buf.constPars()->videoWidth;
	int height = buf.constPars()->videoHeight;

	asel_direct_track((uchar *)buf.constData(), width * height, width , height,
					  v.debug, control.meta, control.panTiltZoomRead,
					  objProp, control.initialize);
	if (control.initialize)
		control.initialize = 0;

	return newOutputBuffer(buf);
}

int TrackAlgorithmElement::semiAutoTrack(const RawBuffer &buf)
{
	return newOutputBuffer(buf);
}

int TrackAlgorithmElement::manualTrack(const RawBuffer &buf)
{
	return newOutputBuffer(buf);
}

int TrackAlgorithmElement::setTrackObjInfo(float x, float y, float w, float h)
{
	control.obj.point_x = x;
	control.obj.point_y = y;
	control.obj.width = w;
	control.obj.height = h;
	algoState = REALLOCATE;
	return 0;
}

