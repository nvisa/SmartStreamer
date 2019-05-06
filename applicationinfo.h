#ifndef APPLICATIONINFO_H
#define APPLICATIONINFO_H

#include <QObject>

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

	bool isBotasFixEnabled();
	bool isBotasDomeEnabled();
	QString getBotasFixPtAddress();
	QString getBotasDomePtAddress();
	int getBotasFixPtGrpcPort();
	int getBotasDomePtGrpcPort();
	QString getBotasFixStreamAddress();
	QString getBotasDomeStreamAddress();
	QString getBotasFixAlgorithms();
protected:
	QJsonObject getSubObj(const QString &objName);
	ApplicationInfo();

};

#endif // APPLICATIONINFO_H
