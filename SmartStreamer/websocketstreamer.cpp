#include "websocketstreamer.h"

#include <lmm/debug.h>

#include <QWebSocket>
#include <QWebSocketServer>

WebsocketStreamer::WebsocketStreamer(QObject *parent)
	: BaseLmmElement(parent)
{
	server = new QWebSocketServer("WsVideoServer", QWebSocketServer::NonSecureMode);
	server->listen(QHostAddress::Any, 1331);
	connect(server, SIGNAL(newConnection()), SLOT(newConnection()));
}

int WebsocketStreamer::processBuffer(const RawBuffer &buf)
{
	lock.lock();
	foreach (QWebSocket *sock, clients) {
		mDebug("Sending to %s", qPrintable(sock->peerAddress().toString()));
		sock->sendBinaryMessage(QByteArray((const char *)buf.constData(), buf.size()));
	}
	lock.unlock();

	return newOutputBuffer(0, buf);
}

void WebsocketStreamer::newConnection()
{
	QWebSocket *sock = server->nextPendingConnection();
	mDebug("New connection from %s", qPrintable(sock->peerAddress().toString()));
	connect(sock, SIGNAL(disconnected()), SLOT(socketDisconnected()));
	lock.lock();
	clients << sock;
	lock.unlock();
}

void WebsocketStreamer::socketDisconnected()
{
	QWebSocket *sock = (QWebSocket *)sender();
	mDebug("Sock %s disconnected", qPrintable(sock->peerAddress().toString()));
	lock.lock();
	clients.removeAll(sock);
	lock.unlock();
	sock->deleteLater();
}
