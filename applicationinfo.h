#ifndef APPLICATIONINFO_H
#define APPLICATIONINFO_H

#include <QObject>
#include <QJsonObject>
#include <QCoreApplication>

class PtzpDriver;
class BaseStreamer;
class BaseAlgorithmElement;

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

	bool isGuiApplication();
	int startPtzpDriver();

	PtzpDriver * getPtzpDriver(int index = 0);
	BaseStreamer * createAppStreamer();
	BaseAlgorithmElement * createAlgorithm(int index);
	BaseAlgorithmElement * createAlgorithmFromJson(const QJsonObject &algo);
	BaseAlgorithmElement * createAlgorithm(const QString &type, int index = 0);
	void checkStartupDelay();

	QString algorithmSet();

protected:
	ApplicationInfo();

	QList<PtzpDriver *> drivers;

};

#endif // APPLICATIONINFO_H
