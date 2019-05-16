#ifndef APPLICATIONINFO_H
#define APPLICATIONINFO_H

#include <QObject>
#include <QCoreApplication>

class PtzpDriver;
class BaseStreamer;

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
	bool isBotasFixEnabled();
	bool isBotasDomeEnabled();
	QString getBotasFixPtAddress();
	QString getBotasDomePtAddress();
	int getBotasFixPtGrpcPort();
	int getBotasDomePtGrpcPort();
	QString getBotasFixStreamAddress();
	QString getBotasDomeStreamAddress();
	QString getBotasFixAlgorithms();

	PtzpDriver * getPtzpDriver(int index = 0);
	BaseStreamer * createAppStreamer();
protected:
	ApplicationInfo();

	QList<PtzpDriver *> drivers;

};

#endif // APPLICATIONINFO_H
