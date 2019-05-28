#include "simpleapiserver.h"

#include <lmm/debug.h>

SimpleApiServer::SimpleApiServer(int port, QObject *parent)
	: SimpleHttpServer(port, parent)
{

}

SimpleApiServer *SimpleApiServer::instance()
{
	static SimpleApiServer *inst = nullptr;
	if (!inst)
		inst = new SimpleApiServer(11111);
	return inst;
}

int SimpleApiServer::handlePostData(const QByteArray &ba)
{
	Q_UNUSED(ba);
	return 0;
}

const QByteArray SimpleApiServer::getFile(const QString filename, QString &mime, QUrl &url)
{
	Q_UNUSED(mime);
	Q_UNUSED(filename);
	emit urlRequested(url);
	return QByteArray(128, '-');
}

QStringList SimpleApiServer::addCustomGetHeaders(const QString &filename)
{
	Q_UNUSED(filename);
	QStringList list;
	list << "Access-Control-Allow-Origin: *";
	return list;
}

