#ifndef WEBSOCKETSTREAMER_H
#define WEBSOCKETSTREAMER_H

#include <lmm/baselmmelement.h>

class QWebSocket;
class QWebSocketServer;

class WebsocketStreamer : public BaseLmmElement
{
	Q_OBJECT
public:
	explicit WebsocketStreamer(QObject *parent = 0);

	int processBuffer(const RawBuffer &buf);
signals:

public slots:
	void newConnection();
	void socketDisconnected();


protected:
	QWebSocketServer *server;
	QList<QWebSocket *> clients;
	QMutex lock;
};

#endif // WEBSOCKETSTREAMER_H
