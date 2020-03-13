#include "moxadriver.h"

#include <QHostAddress>
#include <QTcpSocket>
#include <QDebug>

MoxaDriver::MoxaDriver(QObject *parent) : QObject(parent)
{

}

void MoxaDriver::setTarget(const QString &ip)
{
	QTcpSocket *sock = new QTcpSocket(this);
	connect(sock, SIGNAL(connected()), SLOT(connected()));
	connect(sock, SIGNAL(disconnected()), SLOT(disconnected()));
	connect(sock, SIGNAL(readyRead()), this, SLOT(dataReady()));
	sock->connectToHost(QHostAddress(ip), 4002);
}

void MoxaDriver::connected()
{
	QTcpSocket *sock = (QTcpSocket *)sender();
	if (1) {
		//fov
		const char mes[] = {0x35,0x0a, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xba }; //fov
		qDebug() << "sending" << sizeof(mes);
		sock->write(mes, sizeof(mes));
	} else if (0) {
		const char mes[] = {0x35,0x0a, 0xb0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d }; //fov
		qDebug() << "sending" << sizeof(mes);
		sock->write(mes, sizeof(mes));
	}
	//const char mes[] = {0x35,0x0a, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xba };
	//QString mes = "<IP2TAA:-150000;IP042>";
	//qDebug() << "ok connected" << mes.length();
	//sock->write(mes.toUtf8());
}


void MoxaDriver::disconnected()
{
	QTcpSocket *sock = (QTcpSocket *)sender();
	qDebug() << sock;
}

static void dump(const QByteArray &ba)
{
	for (int i = 0; i < ba.size(); i++)
		qDebug("%d: 0x%x", i, (uchar)ba[i]);
	qDebug() << "-----------------------";
}

void MoxaDriver::dataReady()
{
	QTcpSocket *sock = (QTcpSocket *)sender();
	//dump(sock->readAll());
	//return;
	//qDebug() << 1 << sock->bytesAvailable();
	while (sock->bytesAvailable()) {
		QByteArray ba = sock->read(1);
		//qDebug() << 2;
		if ((uchar)ba[0] == 0xca) {
			//new message
			dump(buffer);
			buffer.clear();
		}
		buffer.append(ba);
	}
	qDebug() << "--";

}

