#include "basealgorithmelement.h"
#include "streamercommon.h"

#include "lmm/debug.h"

#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

BaseAlgorithmElement::BaseAlgorithmElement(QObject *parent)
	: BaseLmmElement(parent)
{
	algoState = UNKNOWN;
	algIndex = 0;
	restarting = false;
	autoStart = false;
}

int BaseAlgorithmElement::init()
{
	QDir::setCurrent("/etc/smartstreamer/motion0_gpu");
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
	mInfo("Current algorithm state is %d", algoState);
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
	case UNKNOWN:
		if (restarting) {
			algoState = INIT;
			restarting = false;
		}
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
	QJsonObject obj = StreamerCommon::readSettingsJSON("/etc/smartstreamer/algodesc.json").object();
	QJsonArray arr = obj["algorithms"].toArray();
	if (arr.size() <= algIndex)
		return -EINVAL;
	QJsonObject node = arr[algIndex].toObject();
	autoStart = node["enabled"].toBool();
	return reloadJson(node);
}

int BaseAlgorithmElement::savetoJson()
{
	QJsonObject obj = StreamerCommon::readSettingsJSON("/etc/smartstreamer/algodesc.json").object();
	QJsonArray arr = obj["algorithms"].toArray();
	if (arr.size() <= algIndex)
		return -EINVAL;
	QJsonObject node = arr[algIndex].toObject();
	QJsonObject aj = resaveJson(node);
	mInfo("Object redefinition %s", qPrintable(QJsonDocument(aj).toJson()));
	if (aj.isEmpty())
		return -1;
	if (algoState >= STOPALGO)
		aj["enabled"] = false;
	else
		aj["enabled"] = true;
	arr[algIndex] = aj;
	obj["algorithms"] = arr;
	StreamerCommon::writeSettingsJSON("/etc/smartstreamer/algodesc.json", QJsonDocument(obj));
	return 0;
}

void BaseAlgorithmElement::setJsonAlgorithmIndex(int index)
{
	algIndex = index;
}

void BaseAlgorithmElement::restart()
{
	restarting = true;
	setState(STOPALGO);
}

void BaseAlgorithmElement::setState(BaseAlgorithmElement::AlgoState state)
{
	/* do not permit recursive algorithm stopping */
	if (state == STOPALGO && algoState != PROCESS) {
		mDebug("error stopping %s", qPrintable(getTypeString()));
		return;
	}
	/* do not permit algorithm init on already running algorithm */
	if (state == INIT && algoState == PROCESS) {
		mDebug("error starting %s", qPrintable(getTypeString()));
		return;
	}
	algoState = state;
	savetoJson();
}

int BaseAlgorithmElement::reloadJson(const QJsonObject &node)
{
	Q_UNUSED(node);
	return 0;
}

QJsonObject BaseAlgorithmElement::resaveJson(const QJsonObject &node)
{
	Q_UNUSED(node);
	return QJsonObject();
}

int BaseAlgorithmElement::processBuffer(const RawBuffer &buf)
{
	return baseAlgorithmProcess(buf);
}

