#include "algorithmelement.h"
#include "algorithmfunctions.h"

AlgorithmElement::AlgorithmElement(QObject *parent)
	: BaseLmmElement(parent)
{
	algHandler.currentActiveAlg = AlgorithmManager::Algorithm::NONE;
	counter = 0;
}

bool AlgorithmElement::setCurrentActiveAlgorithm(AlgorithmElement::Algorithm alg)
{
	algHandler.currentActiveAlg = (AlgorithmManager::Algorithm)alg;
	qDebug() << "algHandler.currentActiveAlg is " << algHandler.currentActiveAlg;
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

void AlgorithmElement::updateAlgorithmParameters(AlgorithmElement::AlgorithmHandler algHand, AlgorithmElement::Algorithm alg)
{
	qDebug() << "Algorithm is " << alg;
	if (alg == Algorithm::MOTION)
	{
		if (algHand.motionA.sensitivity)
			algHandler.motionA.sensitivity	 = algHand.motionA.sensitivity;
	}
	else if (alg == Algorithm::STABILIZATION)
	{
		if (algHand.stabilA.sensitivity)
			algHandler.stabilA.sensitivity = algHand.stabilA.sensitivity;
	}
	else if (alg == Algorithm::TRACKING)
	{
		if (algHand.trackA.trackDuration)
			algHandler.trackA.trackDuration = algHand.trackA.trackDuration;
		if (algHand.trackA.trackScore)
			algHandler.trackA.trackScore = algHand.trackA.trackScore;
	}
	return ;
}

void AlgorithmElement::updateAlgorithmParametersFromManager(AlgorithmManager::AlgorithmHandler algHand, AlgorithmElement::Algorithm alg)
{
	qDebug() << "Algorithm is " << alg;
	if (alg == Algorithm::MOTION)
	{
		if (algHand.motionA.sensitivity)
			algHandler.motionA.sensitivity	 = algHand.motionA.sensitivity;
	}
	else if (alg == Algorithm::STABILIZATION)
	{
		if (algHand.stabilA.sensitivity)
			algHandler.stabilA.sensitivity = algHand.stabilA.sensitivity;
	}
	else if (alg == Algorithm::TRACKING)
	{
		if (algHand.trackA.trackDuration)
			algHandler.trackA.trackDuration = algHand.trackA.trackDuration;
		if (algHand.trackA.trackScore)
			algHandler.trackA.trackScore = algHand.trackA.trackScore;
	} else if (alg == Algorithm::FACE_DETECTION)
	{
		algHandler.faceA.isTileOn = algHand.faceA.isTileOn;
		algHandler.faceA.mode = algHand.faceA.mode;
		algHandler.faceA.xTile = algHand.faceA.xTile;
		algHandler.faceA.yTile = algHand.faceA.yTile;
		algHandler.faceA.isAlignmentOn = algHand.faceA.isAlignmentOn;
	}
	return ;
}

int AlgorithmElement::init()
{
	algHandler.initialize = 1;
	return 0;
}

int AlgorithmElement::clean()
{
	qDebug() << "clean is triggered";
# if 0
	if (algHandler.currentActiveAlg == MOTION)
		asel_via_base_release();
	else if (algHandler.currentActiveAlg == STABILIZATION)
		qDebug() << "....................."; 		//asel_bypass_release(1);
	else if (algHandler.currentActiveAlg == TRACKING)
		asel_direct_track_release();
	else if (algHandler.currentActiveAlg == PANAROMA)
		asel_pan_release();
	else if (algHandler.currentActiveAlg == FACE_DETECTION)
		asel_face_release();
#endif
	return 0;
}

int AlgorithmElement::enableAlg(bool on)
{
	if (on)
		reinit();
	setPassThru(!on);
	return 0;
}

int AlgorithmElement::reinit()
{
	algHandler.initialize = 1;
	return 0;
}

bool AlgorithmElement::setConfigurationElement(AlgorithmManager::AlgorithmHandler algHandler)
{
	qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << algHandler.currentActiveAlg;
	qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << this->algHandler.currentActiveAlg;

	//this->algHandler = algHandler;
	confUnit = algHandler.confUnit;
	this->algHandler.initialize = 1;
	qDebug() << "algHandler.currentActiveAlg is " << algHandler.currentActiveAlg;
	return true;
}

int AlgorithmElement::processBuffer(const RawBuffer &buf)
{
	if (counter % 15 == 0) {
		processAlgorithm(buf);
	}
	counter ++;
	return newOutputBuffer(0, buf);
}
#include <QImage>
int AlgorithmElement::processAlgorithm(const RawBuffer &buf)
{
	qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Inside the process algorithm";
	qDebug() << "active alg is " << algHandler.currentActiveAlg;
	//qDebug() << "algHandler.initialize is " << algHandler.initialize << " and active algorithm is " << algHandler.currentActiveAlg;
	QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);
	float z = hash["current_zoom"].toFloat();
	float *pan_tilt_zoom_read = &z;
	if (algHandler.currentActiveAlg == MOTION) {
		//int bufsize = 1920 * 1080;
		qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << algHandler.motionA.sensitivity;
		/*asel_via_base((uchar *)buf.constData(), confUnit.devProp.frameSize, confUnit.devProp.width , confUnit.devProp.height, confUnit.param.rgb,
					  confUnit.param.record, confUnit.param.shadow,confUnit.param.ill, confUnit.param.debug, confUnit.param.stabilization,confUnit.param.privacy,
					  algHandler.meta,pan_tilt_zoom_read,algHandler.motionA.alarmFlag,algHandler.initialize,algHandler.motionA.sensitivity,algHandler.motionA.classification);
		algHandler.initialize = 0;
		QByteArray ba = QByteArray((char *)algHandler.meta, 4096);
		hash.insert("motion_results", ba);
		((RawBuffer *)&buf)->pars()->metaData = RawBuffer::serializeMetadata(hash);*/
		return 0;
	}
	if (algHandler.currentActiveAlg == STABILIZATION) {
		/*asel_bypass((uchar *)buf.constData(), buf.size(), confUnit.devProp.width, confUnit.devProp.height, confUnit.param.debug,
					confUnit.param.stabilization,confUnit.param.privacy,algHandler.initialize,algHandler.stabilA.sensitivity); // 1 device type olarak girdim
		algHandler.initialize = 0;
*/
		return 0;
	}
	if (algHandler.currentActiveAlg == TRACKING) {
		/*
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
*/
		return 0;
	}
	if (algHandler.currentActiveAlg == PANAROMA) {
		int pan;
		int tilt;
		/*
		asel_pan((uchar *)buf.constData(), buf.size(), confUnit.devProp.width , confUnit.devProp.height, confUnit.param.rgb,
				 confUnit.param.record, confUnit.param.shadow,confUnit.param.ill, confUnit.param.debug, algHandler.meta, pan, tilt);
		algHandler.initialize = 0;
		*/
		return 0;
	}
	if (algHandler.currentActiveAlg == FACE_DETECTION) {
		confUnit.param.debug = 1;
		qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Face detection is triggered" << confUnit.param.debug;
		qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << buf.size();
		int numFace = 0;
		qDebug() << confUnit.devProp.cameraIp;
		int camId = confUnit.devProp.cameraIp.split(".")[3].toInt();
//		qDebug() << "isTileOn: " << algHandler.faceA.isTileOn;
//		qDebug() << "xTile: " << algHandler.faceA.xTile;
//		qDebug() << "yTile: " << algHandler.faceA.yTile;
//		qDebug() << "mode: " << algHandler.faceA.mode;
//		qDebug() << "isAlignmentOn: " << algHandler.faceA.isAlignmentOn;
//		asel_face((uchar *)buf.constData(), numFace, confUnit.param.debug,algHandler.meta,algHandler.initialize, camId);
		algHandler.initialize = 0;

		return 0;
	}
	if (algHandler.currentActiveAlg == NONE)
		return 3;
	return 0;
}
