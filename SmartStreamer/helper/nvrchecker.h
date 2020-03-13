#ifndef NVRCHECKER_H
#define NVRCHECKER_H

#include <QObject>

#include <memory>

class QProcess;
class QTimer;

class NvrChecker : public QObject
{
	Q_OBJECT
public:
	enum class NvrStatus
	{
		Alive,
		Dead,
		NoStatus
	};
	explicit NvrChecker(QObject *parent = 0);
	~NvrChecker();
	void setNvrIp(QString const& ip);
	void setPacketCount(uint count);
	void startPing();
	void stopPing();
	bool isDead();

signals:
	void nvrDead();
	void nvrOnline();

private slots:
	void onNvrDead();
	void onNvrOnline();
	void read(int exitCode);

private:
	std::unique_ptr<QProcess> ping;
	std::unique_ptr<QTimer> timer;
	QString ip = "192.168.50.6";
	int packetCount = 5;
	NvrStatus status = NvrStatus::NoStatus;
};

#endif // NVRCHECKER_H
