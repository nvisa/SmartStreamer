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
	bool isBotasFixEnabled();
	bool isBotasDomeEnabled();
	QString getBotasFixPtAddress();
	QString getBotasDomePtAddress();
	int getBotasFixPtGrpcPort();
	int getBotasDomePtGrpcPort();
	QString getBotasFixStreamAddress();
	QString getBotasDomeStreamAddress();
	QString getBotasFixAlgorithms();
	int startPtzpDriver();

	PtzpDriver * getPtzpDriver(int index = 0);
	BaseStreamer * createAppStreamer();
	BaseAlgorithmElement * createAlgorithm(int index);
	BaseAlgorithmElement * createAlgorithmFromJson(const QJsonObject &algo);
	BaseAlgorithmElement * createAlgorithm(const QString &type, int index = 0);

	QString algorithmSet();

protected:
	ApplicationInfo();

	QList<PtzpDriver *> drivers;

};

#endif // APPLICATIONINFO_H
