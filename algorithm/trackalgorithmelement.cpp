#include "trackalgorithmelement.h"

#include "lmm/debug.h"

TrackAlgorithmElement::TrackAlgorithmElement(QObject *parent)
	: BaseAlgorithmElement(parent)
{

}

int TrackAlgorithmElement::init()
{
	v = BaseAlgorithmCommon::instance()->getAlgoParameters();
	control.initialize = 1;
	return BaseAlgorithmElement::init();
}

int TrackAlgorithmElement::reallocate()
{

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

}

int TrackAlgorithmElement::release()
{

}

int TrackAlgorithmElement::autoTrack(const RawBuffer &buf)
{

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
