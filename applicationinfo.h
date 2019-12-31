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

class ApplicationInfo : public QObject
{
	Q_OBJECT
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
		MGEO_SWIR,
		HTR_SWIR,
		OEM4K
	};

	bool isGuiApplication();
	int startPtzpDriver();
	Platform getApplicationPlatform();
	QString getAselsanPlatform();
	PtzpDriver * getPtzpDriver(int index = 0);
	QJsonObject getPtzpObject(int index = 0);
	BaseStreamer * createAppStreamer();
	BaseAlgorithmElement * createAlgorithmFromJson(const QJsonObject &algo);
	BaseAlgorithmElement * createAlgorithm(const QString &type, int index = 0);
	void checkStartupDelay();
	InDeviceTest * getIDT();
	int init();

	QString algorithmSet();
	BaseAlgorithmElement * getAlgorithmInstance(int index);
	BaseAlgorithmElement * getAlgorithmInstance(const QString &type, int index);


	LifeTimeTracker *lifetime;
	qint64 getLifeTime();

public slots:
	void timeoutApp();

protected:
	ApplicationInfo();

	InDeviceTest *idt;
	QList<PtzpDriver *> drivers;
	QList<BaseAlgorithmElement*> algorithms;
	QHash<QString, QList<BaseAlgorithmElement*> > algoNameMapping;

};

#endif // APPLICATIONINFO_H
