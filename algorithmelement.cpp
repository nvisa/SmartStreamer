#include "algorithmelement.h"
#include "algorithmfunctions.h"

AlgorithmElement::AlgorithmElement(QObject *parent)
	: BaseLmmElement(parent)
{

}

bool AlgorithmElement::setCurrentActiveAlgorithm(AlgorithmElement::Algorithm alg)
{
	algHandler.currentActiveAlg = (AlgorithmManager::Algorithm)alg;
	return true;
}

AlgorithmManager::PTZinformation AlgorithmElement::forwardPTZaction(uchar meta[])
{
	int   sei_direction = meta[10];
	ptzInfo.pan  = (float)meta[11];
	ptzInfo.tilt = (float)meta[12];
	if (sei_direction == 0) {
		ptzInfo.action = AlgorithmManager::PAN_STOP;
	} else if (sei_direction == 2)
	{
		ptzInfo.action = AlgorithmManager::PAN_RIGHT;
	} else if (sei_direction == 4) {
		ptzInfo.action = AlgorithmManager::PAN_LEFT;
	} else if (sei_direction == 16) {
		ptzInfo.action = AlgorithmManager::TILT_UP;
	} else if (sei_direction == 8) {
		ptzInfo.action = AlgorithmManager::TILT_DOWN;
	} else if (sei_direction == 18){
		ptzInfo.action = AlgorithmManager::PAN_RIGHT_TILT_UP;
	} else if (sei_direction == 20){
		ptzInfo.action = AlgorithmManager::PAN_RIGHT_TILT_DOWN;
	} 	else if (sei_direction == 10){
		ptzInfo.action = AlgorithmManager::PAN_LEFT_TILT_UP;
	} else if (sei_direction == 12){
		ptzInfo.action = AlgorithmManager::PAN_LEFT_TILT_DOWN;
	} else
		ptzInfo.action = AlgorithmManager::PAN_TILT_POS;
	return ptzInfo;
}

bool AlgorithmElement::setConfigurationElement(AlgorithmManager::AlgorithmHandler algHandler)
{
	this->algHandler = algHandler;
	confUnit = algHandler.confUnit;
	this->algHandler.initialize = 1;
	return true;
}

int AlgorithmElement::processBuffer(const RawBuffer &buf)
{
	qDebug() << "************************************************ buf size " << buf.size();
	processAlgorithm(buf);
	return newOutputBuffer(0, buf);
}

int AlgorithmElement::processAlgorithm(const RawBuffer &buf)
{

	QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);
	float z = hash["current_zoom"].toFloat();
	float *pan_tilt_zoom_read = &z;
	if (algHandler.currentActiveAlg == MOTION) {
		//int bufsize = 1920 * 1080;
		asel_via_base((uchar *)buf.constData(), confUnit.devProp.frameSize, confUnit.devProp.width , confUnit.devProp.height, confUnit.param.rgb,
					  confUnit.param.record, confUnit.param.shadow,confUnit.param.ill, confUnit.param.debug, confUnit.param.stabilization,confUnit.param.privacy,
					  algHandler.meta,pan_tilt_zoom_read,algHandler.motionA.alarmFlag,algHandler.initialize,algHandler.motionA.sensitivity);
		algHandler.initialize = 0;
		QByteArray ba = QByteArray((char *)algHandler.meta, 4096);
		hash.insert("motion_results", ba);
		((RawBuffer *)&buf)->pars()->metaData = RawBuffer::serializeMetadata(hash);
		return 0;
	}
	if (algHandler.currentActiveAlg == STABILIZATION) {
		asel_bypass((uchar *)buf.constData(), buf.size(), confUnit.devProp.width, confUnit.devProp.height, confUnit.param.debug,
					confUnit.param.stabilization,confUnit.param.privacy,algHandler.initialize,algHandler.stabilA.sensitivity); // 1 device type olarak girdim
		algHandler.initialize = 0;

		return 0;
	}
	if (algHandler.currentActiveAlg == TRACKING) {
		float objProp[4];
		objProp[0] = algHandler.trackA.objPointX;
		objProp[1] = algHandler.trackA.objPointY;
		objProp[2] = algHandler.trackA.objWidth;
		objProp[3] = algHandler.trackA.objHeight;
		asel_direct_track((uchar *)buf.constData(), buf.size(), confUnit.devProp.width , confUnit.devProp.height, confUnit.param.record,
						  confUnit.param.debug, algHandler.meta, 1,pan_tilt_zoom_read,objProp,algHandler.initialize);
		algHandler.initialize = 0;
		QByteArray ba = QByteArray((char *)algHandler.meta, 4096);
		hash.insert("track_results", ba);
		((RawBuffer *)&buf)->pars()->metaData = RawBuffer::serializeMetadata(hash);
		forwardPTZaction(algHandler.meta);

		return 0;
	}
	if (algHandler.currentActiveAlg == PANAROMA) {
		int pan;
		int tilt;
		asel_pan((uchar *)buf.constData(), buf.size(), confUnit.devProp.width , confUnit.devProp.height, confUnit.param.rgb,
				 confUnit.param.record, confUnit.param.shadow,confUnit.param.ill, confUnit.param.debug, algHandler.meta, pan, tilt);
		algHandler.initialize = 0;
		return 0;
	}
	if (algHandler.currentActiveAlg == FACE_DETECTION) {
		return 0;
	}
	if (algHandler.currentActiveAlg == NONE)
		return 3;
	return 0;
}
