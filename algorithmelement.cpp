#include "algorithmelement.h"
#include "algorithmfunctions.h"
#include "QFile"

#include <QImage>
#include "algorithmmanager.h"

AlgorithmElement::AlgorithmElement(QObject *parent)
	: BaseLmmElement(parent)
{
	algHandlerEl.currentActiveAlg = AlgorithmElement::NONE;
	counter = 0;
	ptManager = NULL;
}

bool AlgorithmElement::setConfigurationElement(AlgorithmElement::AlgorithmElementHandler algHandler,AlgorithmElement::Algorithm alg)
{
	this->algHandlerEl = algHandler;
	this->algHandlerEl.confUnit = algHandler.confUnit;
	return true;
}

bool AlgorithmElement::setCurrentActiveAlgorithm(AlgorithmElement::Algorithm alg)
{
	algHandlerEl.currentActiveAlg = alg;
	this->algHandlerEl.initialize = 1;
	return true;
}

PTZinformation AlgorithmElement::forwardPTZaction(uchar meta[])
{
	int sei_direction = meta[10];
	ptzInfo.pan  = (float)meta[11];
	ptzInfo.tilt = (float)meta[12];
	ptzInfo.zoom = 0;
	if (sei_direction == 0) {
		ptzInfo.action = PAN_STOP;
	} else if (sei_direction == 2)
	{
		ptzInfo.action = PAN_RIGHT;
	} else if (sei_direction == 4) {
		ptzInfo.action = PAN_LEFT;
	} else if (sei_direction == 16) {
		ptzInfo.action = TILT_UP;
	} else if (sei_direction == 8) {
		ptzInfo.action = TILT_DOWN;
	} else if (sei_direction == 18){
		ptzInfo.action = PAN_RIGHT_TILT_UP;
	} else if (sei_direction == 20){
		ptzInfo.action = PAN_RIGHT_TILT_DOWN;
	} 	else if (sei_direction == 10){
		ptzInfo.action = PAN_LEFT_TILT_UP;
	} else if (sei_direction == 12){
		ptzInfo.action = PAN_LEFT_TILT_DOWN;
	} else
		ptzInfo.action = PAN_TILT_POS;
	return ptzInfo;
}

void AlgorithmElement::updateAlgorithmParameters(AlgorithmElementHandler algHand, AlgorithmElement::Algorithm alg)
{
	if (alg == Algorithm::MOTION)
	{
		if (algHand.motionA.sensitivity)
			algHandlerEl.motionA.sensitivity = algHand.motionA.sensitivity;
	}
	else if (alg == Algorithm::STABILIZATION)
	{
		if (algHand.stabilA.sensitivity)
			algHandlerEl.stabilA.sensitivity = algHand.stabilA.sensitivity;
	}
	else if (alg == Algorithm::TRACKING)
	{
		if (algHand.trackA.trackDuration)
			algHandlerEl.trackA.trackDuration = algHand.trackA.trackDuration;
		if (algHand.trackA.trackScore)
			algHandlerEl.trackA.trackScore = algHand.trackA.trackScore;
	}
	else if (alg == Algorithm::FACE_DETECTION)
	{
		algHandlerEl.faceA.isTileOn = algHand.faceA.isTileOn;
		algHandlerEl.faceA.mode = algHand.faceA.mode;
		algHandlerEl.faceA.xTile = algHand.faceA.xTile;
		algHandlerEl.faceA.yTile = algHand.faceA.yTile;
		algHandlerEl.faceA.isAlignmentOn = algHand.faceA.isAlignmentOn;
	}
	return;
}

void AlgorithmElement::setAlgorithmManager(AlgorithmManager *algMan)
{
	ptManager = algMan;
}

int AlgorithmElement::init()
{
	algHandlerEl.initialize = 1;
	return 0;
}

int AlgorithmElement::clean()
{
	if (algHandlerEl.currentActiveAlg == MOTION)
		asel_via_base_release();
	else if (algHandlerEl.currentActiveAlg == STABILIZATION)
		qDebug() << " " ; //asel_bypass_release(1);
	else if (algHandlerEl.currentActiveAlg == TRACKING)
		asel_direct_track_release();
	else if (algHandlerEl.currentActiveAlg == PANAROMA)
		asel_pan_release();
	else if (algHandlerEl.currentActiveAlg == FACE_DETECTION)
		asel_face_release();
	algHandlerEl.initialize = 1;
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
	return 0;
}

int AlgorithmElement::skipFrameRate()
{
	QString info;
	QFile file;
	file.setFileName("algorithm.json");
	if (file.exists()) {
		file.open(QIODevice::ReadOnly | QIODevice::Text);
	} else {
		//		mDebug("Algorithm.json file is not found");
		return -1;
	}
	info = file.readAll();
	file.close();

	QJsonDocument doc = QJsonDocument::fromJson(info.toUtf8());
	QJsonObject obj_info = doc.object();

	QJsonValue sec_alg = obj_info.value(QString("algorithms"));
	QJsonObject itemized_sec_alg = sec_alg.toObject();
	QJsonObject face_params = itemized_sec_alg["face_detection"].toObject();
	int skipFrame = face_params["skipFrame"].toInt();
	return skipFrame;
}

int AlgorithmElement::processBuffer(const RawBuffer &buf)
{
	if (algHandlerEl.currentActiveAlg == FACE_DETECTION)
	{
		int skipFrame = skipFrameRate();
		if (counter % skipFrame == 0)
		{
			processAlgorithm(buf);
		}
		counter ++;
	} else
	{
		processAlgorithm(buf);
	}
	return newOutputBuffer(0, buf);
}

int AlgorithmElement::processAlgorithm(const RawBuffer &buf)
{
	QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);
	float z = hash["current_zoom"].toFloat();
	float *pan_tilt_zoom_read = &z;
	if (algHandlerEl.currentActiveAlg == MOTION) {
		asel_via_base((uchar *)buf.constData(), algHandlerEl.confUnit.devProp.frameSize, algHandlerEl.confUnit.devProp.width , algHandlerEl.confUnit.devProp.height, algHandlerEl.confUnit.param.rgb,
					  algHandlerEl.confUnit.param.record, algHandlerEl.confUnit.param.shadow,algHandlerEl.confUnit.param.ill, algHandlerEl.confUnit.param.debug, algHandlerEl.confUnit.param.stabilization,algHandlerEl.confUnit.param.privacy,
					  algHandlerEl.meta,pan_tilt_zoom_read,algHandlerEl.motionA.alarmFlag,algHandlerEl.initialize,algHandlerEl.motionA.sensitivity);
		algHandlerEl.initialize = 0;
		QByteArray ba = QByteArray((char *)algHandlerEl.meta, 4096);
		hash.insert("motion_results", ba);
		((RawBuffer *)&buf)->pars()->metaData = RawBuffer::serializeMetadata(hash);
		return 0;
	}
	if (algHandlerEl.currentActiveAlg == STABILIZATION) {
		asel_bypass((uchar *)buf.constData(), buf.size(), algHandlerEl.confUnit.devProp.width, algHandlerEl.confUnit.devProp.height, algHandlerEl.confUnit.param.debug,
					algHandlerEl.confUnit.param.stabilization,algHandlerEl.confUnit.param.privacy,algHandlerEl.initialize,algHandlerEl.stabilA.sensitivity); // 1 device type olarak girdim
		algHandlerEl.initialize = 0;
		return 0;
	}
	if (algHandlerEl.currentActiveAlg == TRACKING) {
		float objProp[4];
		objProp[0] = algHandlerEl.trackA.objPointX;
		objProp[1] = algHandlerEl.trackA.objPointY;
		objProp[2] = algHandlerEl.trackA.objWidth;
		objProp[3] = algHandlerEl.trackA.objHeight;
		asel_direct_track((uchar *)buf.constData(), buf.size(), algHandlerEl.confUnit.devProp.width , algHandlerEl.confUnit.devProp.height, algHandlerEl.confUnit.param.record,
						  algHandlerEl.confUnit.param.debug, algHandlerEl.meta,pan_tilt_zoom_read,objProp,algHandlerEl.initialize);
		algHandlerEl.initialize = 0;
		QByteArray ba = QByteArray((char *)algHandlerEl.meta, 4096);
		hash.insert("track_results", ba);
		((RawBuffer *)&buf)->pars()->metaData = RawBuffer::serializeMetadata(hash);

		PTZinformation ptinfo = forwardPTZaction(algHandlerEl.meta);
		if (ptManager) {
			ptManager->setPT(ptinfo);
			ptManager->setZoom(ptinfo.zoom);
		}
		return 0;
	}
	if (algHandlerEl.currentActiveAlg == PANAROMA) {
		int pan;
		int tilt;
		asel_pan((uchar *)buf.constData(), buf.size(), algHandlerEl.confUnit.devProp.width , algHandlerEl.confUnit.devProp.height, algHandlerEl.confUnit.param.rgb,
				 algHandlerEl.confUnit.param.record, algHandlerEl.confUnit.param.shadow,algHandlerEl.confUnit.param.ill, algHandlerEl.confUnit.param.debug, algHandlerEl.meta, pan, tilt);
		algHandlerEl.initialize = 0;
		return 0;
	}
	if (algHandlerEl.currentActiveAlg == FACE_DETECTION) {
		algHandlerEl.confUnit.param.debug = 1;
		int numFace = 0;
		int camId = algHandlerEl.confUnit.devProp.cameraIp.split(".")[3].toInt();
		asel_face((uchar *)buf.constData(), numFace, algHandlerEl.confUnit.param.debug,algHandlerEl.meta,algHandlerEl.initialize, camId);
		algHandlerEl.initialize = 0;
		return 0;
	}
	if (algHandlerEl.currentActiveAlg == NONE)
		return 3;
	return 0;
}

