#ifndef SIMPLEAPISERVER_H
#define SIMPLEAPISERVER_H

#include <simplehttpserver.h>

#include <QUrl>

class SimpleApiServer : public SimpleHttpServer
{
	Q_OBJECT
public:
	static SimpleApiServer * instance();

	virtual int handlePostData(const QByteArray &ba);
	virtual const QByteArray getFile(const QString filename, QString &mime, QUrl &url);

signals:
	void urlRequested(QUrl url);

protected:
	explicit SimpleApiServer(int port, QObject *parent = 0);

	virtual QStringList addCustomGetHeaders(const QString &filename);
};

#endif // SIMPLEAPISERVER_H
