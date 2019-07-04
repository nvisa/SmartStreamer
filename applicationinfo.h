#ifndef APPLICATIONINFO_H
#define APPLICATIONINFO_H

#include <QHash>
#include <QObject>
#include <QJsonObject>
#include <QCoreApplication>

class PtzpDriver;
class BaseStreamer;
class InDeviceTest;
class BaseAlgorithmElement;
class LifeTimeTracker;

class ApplicationInfo
{
public:
	static ApplicationInfo* instance()
	{
		static ApplicationInfo* inst = new ApplicationInfo();
		if (inst == 0)
			inst = new ApplicationInfo();
		return inst;
	}

	enum Platform {
		GENERIC,
		BOTAS_FIX,
		BOTAS_DOME,
		ARYA_ORION,
		KAYI_SAHINGOZ,
		YAMGOZ,
		TBGTH,
		FLIR_ORION,
		MGEO_SWIR
	};

	bool isGuiApplication();
	int startPtzpDriver();
	Platform getApplicationPlatform();
	PtzpDriver * getPtzpDriver(int index = 0);
	QJsonObject getPtzpObject(int index = 0);
	BaseStreamer * createAppStreamer();
	BaseAlgorithmElement * createAlgorithm(int index);
	BaseAlgorithmElement * createAlgorithmFromJson(const QJsonObject &algo);
	BaseAlgorithmElement * createAlgorithm(const QString &type, int index = 0);
	void checkStartupDelay();
	InDeviceTest * getIDT();

	QString algorithmSet();
	QHash<int, BaseAlgorithmElement*> algoIndexes;

	LifeTimeTracker *lifetime;
	qint64 getLifeTime();
protected:
	ApplicationInfo();

	InDeviceTest *idt;
	QList<PtzpDriver *> drivers;

};

#endif // APPLICATIONINFO_H
