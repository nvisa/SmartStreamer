#include "nvrchecker.h"

#include <QRegularExpression>
#include <QElapsedTimer>
#include <QProcess>
#include <QDebug>
#include <QTimer>

static bool hasReceivedAllPackages(QString const& response)
{
	static QRegularExpression networkUnreachableExp("connect: Network is unreachable");
	if (networkUnreachableExp.match(response, 0).hasMatch()) {
		qDebug() << "Network is unreachable!!!!!!!";
		return false;
	}
	static QRegularExpression packetsTransmittedExp("[0-9]+ packets transmitted");
	static QRegularExpression packetsReceivedExp(" [0-9]+ received");
	static QRegularExpression captureInt("[0-9]+ ");
	QRegularExpressionMatch transmittedMatch = packetsTransmittedExp.match(response, 0);
	if (!transmittedMatch.hasMatch())
		return false;
	QRegularExpressionMatch receivedMatch = packetsReceivedExp.match(response, 0);
	if (!receivedMatch.hasMatch())
		return false;
	int transmitted = captureInt.match(transmittedMatch.captured(), 0).captured().toInt();
	int received = captureInt.match(receivedMatch.captured(), 0).captured().toInt();
	return transmitted == received;
}

static bool isNvrAlive(int exitCode, QString const& response)
{
	if (response.isEmpty()) {
		return false;
	} else if (exitCode != 0) {
		return false;
	} else {
		return hasReceivedAllPackages(response);
	}
}

NvrChecker::NvrChecker(QObject *parent)
	: QObject(parent),
	  ping(new QProcess),
	  timer(new QTimer)
{
	connect(ping.get(), SIGNAL(finished(int)), this, SLOT(read(int)));
	connect(this, &NvrChecker::nvrDead, this, &NvrChecker::onNvrDead);
	connect(this, &NvrChecker::nvrOnline, this, &NvrChecker::onNvrOnline);
}

NvrChecker::~NvrChecker()
{
}

void NvrChecker::setNvrIp(const QString &ip)
{
	this->ip = ip;
}

void NvrChecker::setPacketCount(uint count)
{
	packetCount = count;
}

void NvrChecker::startPing()
{
	ping->start(QString("ping ").append(ip).append(" -c %1").arg(packetCount), QProcess::ReadOnly);
}

void NvrChecker::stopPing()
{
	disconnect(this, &NvrChecker::nvrDead, this, &NvrChecker::onNvrDead);
	disconnect(this, &NvrChecker::nvrOnline, this, &NvrChecker::onNvrOnline);
	disconnect(ping.get(), SIGNAL(finished(int)), this, SLOT(read(int)));
	ping->terminate();
	ping->waitForFinished(1000);
}

bool NvrChecker::isDead()
{
	return status == NvrStatus::Dead;
}

void NvrChecker::onNvrDead()
{
	if (status != NvrStatus::Dead) {
		qDebug() << "No stable response from" << ip;
	}
	status = NvrStatus::Dead;
}

void NvrChecker::onNvrOnline()
{
	if (status != NvrStatus::Alive) {
		qDebug() << "Connection to Nvr with ip" << ip << "is established";
	}
	status = NvrStatus::Alive;
}

void NvrChecker::read(int exitCode)
{
	QString response = ping->readAll().data();
	if (isNvrAlive(exitCode, response)) {
		emit nvrOnline();
		startPing();
	} else {
		emit nvrDead();
		ping->close();
		QTimer::singleShot(packetCount * 1000, this, &NvrChecker::startPing);
	}
}
