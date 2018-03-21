#ifndef MOXADRIVER_H
#define MOXADRIVER_H

#include <QObject>

class MoxaDriver : public QObject
{
	Q_OBJECT
public:
	explicit MoxaDriver(QObject *parent = 0);

	void setTarget(const QString &ip);
signals:

public slots:
	void connected();
	void disconnected();
	void dataReady();
protected:
	QByteArray buffer;
};

#endif // MOXADRIVER_H
