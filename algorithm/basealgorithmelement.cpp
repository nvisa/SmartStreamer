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

static int saveJson(const QString &filename, const QJsonDocument &doc)
{
	QFile f(filename);
	if (!f.open(QIODevice::WriteOnly)) {
		qDebug() << "File opening error: " << errno << filename;
		return -1;
	}
	f.write(doc.toJson());
	f.close();
	return 0;
}

BaseAlgorithmElement::BaseAlgorithmElement(QObject *parent)
	: BaseLmmElement(parent)
{
	algoState = UNKNOWN;
	algIndex = 0;
	restarting = false;
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
	QJsonObject obj = readJson("/etc/smartstreamer/algodesc.json");
	QJsonArray arr = obj["algorithms"].toArray();
	if (arr.size() <= algIndex)
		return -EINVAL;
	QJsonObject node = arr[algIndex].toObject();
	if (node["enabled"].toBool())
		setState(INIT);
	return reloadJson(node);
}

int BaseAlgorithmElement::savetoJson()
{
	QJsonObject obj = readJson("/etc/smartstreamer/algodesc.json");
	QJsonArray arr = obj["algorithms"].toArray();
	if (arr.size() <= algIndex)
		return -EINVAL;
	QJsonObject node = arr[algIndex].toObject();
	QJsonObject aj = resaveJson(node);
	mInfo("Object redefinition %s", qPrintable(QJsonDocument(aj).toJson()));
	if (aj.isEmpty())
		return -1;
	arr[algIndex] = aj;
	obj["algorithms"] = arr;
	saveJson("/etc/smartstreamer/algodesc.json", QJsonDocument(obj));
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

