#include "facealgorithmelement.h"

#include <lmm/debug.h>

#include "algorithmfunctions.h"

FaceAlgorithmElement::FaceAlgorithmElement(QObject *parent)
	: BaseAlgorithmElement(parent)
{
	algoState = UNKNOWN;
}

int FaceAlgorithmElement::init()
{
	control.initialize = 1;
	control.camId = 12;
	control.counter = 0;
	control.debug = 1;
	return 0;
}

int FaceAlgorithmElement::reallocate()
{
	return 0;
}

int FaceAlgorithmElement::processAlgo(const RawBuffer &buf)
{
	int numFace = 0;
	if(control.counter % faceParams.skipFrame == 0)
	{
//		qDebug() << "Count " << control.counter;
		asel_face((uchar *)buf.constData(), numFace, control.debug, control.meta, control.initialize, control.camId, faceParams);
		control.counter = 0;
	}
	control.counter++;
	control.numface = numFace;
	if (control.initialize)
	{
		control.initialize = 0;
	}
	return 0;
}

QString FaceAlgorithmElement::getTypeString()
{
	return "faceDetection";
}

FaceDetection FaceAlgorithmElement::getFaceParams()
{
	return faceParams;
}

void FaceAlgorithmElement::setFaceParams(const QJsonObject &node)
{
	faceParams.detectionModel = node["detectionModel"].toString().toUtf8().constData();
	faceParams.allignmentModel = node["allignmentModel"].toString().toUtf8().constData();
	faceParams.tiling = node["tiling"].toBool();
	faceParams.includeFullFrame = node["includeFullFrame"].toBool();
	faceParams.xTile = node["xTile"].toInt();
	faceParams.yTile = node["yTile"].toInt();
	faceParams.displayMode = node["displayMode"].toString().toUtf8().constData();
	faceParams.rectangleColor = node["rectangleColor"].toString().toUtf8().constData();
	faceParams.allignment = node["allignment"].toBool();
	faceParams.saveFaces = node["saveFaces"].toBool();
	faceParams.saveOptions = node["saveOptions"].toString().toUtf8().constData();
	faceParams.saveDir = node["saveDir"].toString().toUtf8().constData();
	faceParams.allignmentErrorThreshold = node["allignmentErrorThreshold"].toInt();
	faceParams.scoreThreshold = node["scoreThreshold"].toDouble();
	faceParams.sizePercentageUpperLimitX = node["sizePercentageUpperLimitX"].toDouble();
	faceParams.sizePercentageUpperLimitY = node["sizePercentageUpperLimitY"].toDouble();
	faceParams.sizePercentageLowerLimitX = node["sizePercentageLowerLimitX"].toDouble();
	faceParams.sizePercentageLowerLimitY = node["sizePercentageLowerLimitY"].toDouble();
	faceParams.trackFaces = node["trackFaces"].toBool();
	faceParams.bestFaceNum = node["bestFaceNum"].toInt();
	faceParams.enlargePercentage = node["enlargePercentage"].toInt();
	faceParams.saveFrameCount = node["saveFrameCount"].toInt();
	faceParams.eraseFaceThresh = node["eraseFaceThresh"].toInt();
	faceParams.validFaceThresh = node["validFaceThresh"].toInt();
	faceParams.roiRatioX1 = node["roiRatioX1"].toDouble();
	faceParams.roiRatioY1 = node["roiRatioY1"].toDouble();
	faceParams.roiRatioX2 = node["roiRatioX2"].toDouble();
	faceParams.roiRatioY2 = node["roiRatioY2"].toDouble();
	faceParams.blurThreshold = node["blurThreshold"].toInt();
	faceParams.skipFrame = node["skipFrame"].toInt();
}

int FaceAlgorithmElement::reloadJson(const QJsonObject &node)
{
	setFaceParams(node);
	return 0;
}

int FaceAlgorithmElement::release()
{
	asel_face_release();
	return 0;
}



