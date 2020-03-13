#ifndef PEERCHECK_H
#define PEERCHECK_H

#include <QString>

class PeerCheck
{
public:
	PeerCheck();

	static bool isAlive(QString ip);
	static void leaveEvpuAlone(QString ip);
};

#endif // PEERCHECK_H
