#include "basealgorithmelement.h"

#include "lmm/debug.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

static QJsonObject readJson(const QString &filename)
{
	QJsonObject obj;
	QFile f(filename);
	if (!f.open(QIODevice::ReadOnly)) {
		qDebug() << "File opening error: " << errno << filename;
		return obj;
	}
	QByteArray ba = f.readAll();
	f.close();
	QJsonDocument doc = QJsonDocument::fromJson(ba);
	obj = doc.object();
	return obj;
}

BaseAlgorithmElement::BaseAlgorithmElement(QObject *parent)
	: BaseLmmElement(parent)
{
	algoState = UNKNOWN;
	algIndex = 0;
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

QString BaseAlgorithmElement::getTypeString()
{
	return "dummy";
}

int BaseAlgorithmElement::reloadJson()
{
	QJsonObject obj = readJson("/etc/smartstreamer/algodesc.json");
	QJsonArray arr = obj["algorithms"].toArray();
	if (arr.size() <= algIndex)
		return -EINVAL;
	QJsonObject node = arr[algIndex].toObject();
	if (node["enabled"].toBool())
		setState(INIT);
	return reloadJson(node);
}

void BaseAlgorithmElement::setJsonAlgorithmIndex(int index)
{
	algIndex = index;
}

int BaseAlgorithmElement::reloadJson(const QJsonObject &node)
{
	Q_UNUSED(node);
	return 0;
}

int BaseAlgorithmElement::processBuffer(const RawBuffer &buf)
{
	return baseAlgorithmProcess(buf);
}

