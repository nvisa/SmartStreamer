#include <QCoreApplication>

#include <lmm/lmmcommon.h>

#include <ecl/debug.h>
#include <ecl/net/remotetcpconnection.h>

#include "smartstreamer.h"
#include "moxadriver.h"

static inline QString getArg(const QString &aname, QCoreApplication *a)
{
	int ind = a->arguments().indexOf(aname);
	if (ind < 0)
		return "";
	if (ind + 1 >= a->arguments().size())
		return "";
	return a->arguments()[ind + 1];
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	LmmCommon::init();
	ecl::initDebug();

	//MoxaDriver m;
	//m.setTarget("50.23.169.213");
#if 1
	SmartStreamer s;
	QString url = getArg("--rtsp-url", &a);
	url = "rtsp://50.23.169.211/multicaststream_ch1_stream1";
	if (!url.isEmpty())
		s.setupRtspClient(url);
	s.start();
#endif
	return a.exec();
}

