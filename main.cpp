#include <QDir>
#include <QApplication>

#include <lmm/lmmcommon.h>

#include <ecl/debug.h>
#include <ecl/net/remotetcpconnection.h>

#include "smartstreamer.h"
#include "moxadriver.h"

class MyQApp : public QApplication
{
public:
	MyQApp(int &argc, char **argv)
		: QApplication(argc, argv)
	{

	}

	bool notify(QObject *r, QEvent *ev)
	{
		QElapsedTimer t; t.start();
		bool res = QApplication::notify(r, ev);
		if (t.elapsed() > 100)
			qDebug() << "shite" << r << ev << r->parent() << r->parent()->objectName();
		return res;
	}
};

static inline QString getArg(const QString &aname, QCoreApplication *a)
{
	int ind = a->arguments().indexOf(aname);
	if (ind < 0)
		return "";
	if (ind + 1 >= a->arguments().size())
		return "";
	return a->arguments()[ind + 1];
}

static bool hasArg(const QString &aname, QCoreApplication *a)
{
	int ind = a->arguments().indexOf(aname);
	if (ind < 0)
		return false;
	return true;
}

static QList<QPair<QString, QString> > help;
static inline QString getCommandlineParameter(const QString &par, QApplication *a, const QString &desc, const QVariant &def)
{
	help << QPair<QString, QString>(par, desc);
	if (hasArg(par, a))
		return getArg(par, a);
	return def.toString();
}

static void printHelp()
{
	for (int i = 0; i < help.size(); i++) {
		QPair<QString, QString> p = help[i];
		qDebug("\t%s: %s", qPrintable(p.first), qPrintable(p.second));
	}
}

#define getCmdParInt(_x, _str, _desc) _x = getCommandlineParameter(_str, &a, _desc, _x).toInt(0, 0)
#define getCmdParStr(_x, _str, _desc) _x = getCommandlineParameter(_str, &a, _desc, _x)

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QDir::setCurrent(a.applicationDirPath());

	LmmCommon::init();
	ecl::initDebug();

	//MoxaDriver m;
	//m.setTarget("50.23.169.213");
#if 1
	SmartStreamer s;
	SmartStreamer::Parameters pars;
	getCmdParInt(pars.decWidth, "--dec-width", "Incoming RTSP video width, default automatic");
	getCmdParInt(pars.decHeight, "--dec-height", "Incoming RTSP video height, default automatic");
	getCmdParInt(pars.decBufferCount, "--dec-buffer-count", "Soft decoder buffer count, default 60");
	getCmdParInt(pars.decOutputInFps, "--dec-out-infps", "Soft decoder output rate reduction input fps, default 0");
	getCmdParInt(pars.decOutputOutFps, "--dec-out-outfps", "Soft decoder output rate reduction output fps, default 0");
	getCmdParInt(pars.secWidth, "--second-stream-width", "Secondary output resolution width, default 0");
	getCmdParInt(pars.secHeight, "--second-stream-height", "Secondary output resolution height, default 0");
	getCmdParInt(pars.rgbMainWidth, "--rgb-stream-width", "RGB analytics stream resolution width, default 640");
	getCmdParInt(pars.rgbMainHeight, "--rgb-stream-height", "RGB analytics stream resolution height, default 360");
	getCmdParInt(pars.rtpBufferDuration, "--rtp-buffer-duration", "Incoming RTP buffer duration in miliseconds, default automatic");
	getCmdParStr(pars.rtspClientUser, "--rtsp-client-user", "RTSP client username, default 'none'");
	getCmdParStr(pars.rtspClientPass, "--rtsp-client-pass", "RTSP client password, default 'none'");
	getCmdParStr(pars.rtspServerUser, "--rtsp-server-user", "RTSP server username, default 'none'");
	getCmdParStr(pars.rtspServerPass, "--rtsp-server-pass", "RTSP server password, default 'none'");
	getCmdParInt(pars.enableMoxaHacks, "--moxa-hacks", "Enable MOXA related various hacks, default '0'");
	getCmdParInt(pars.pipelineFlags, "--pipeline-flags", "Pipeline customization flags, default 0xffffffff");
	if (pars.pipelineFlags == 0)
		pars.pipelineFlags = 0xffffffff; //hex fix
	s.pars = pars;

	QString url = getCommandlineParameter("--rtsp-url", &a, "RTSP camera URL, default 'rtsp://10.5.176.65/Streaming/Channels/1'", "");

	if (a.arguments().contains("--help")) {
		printHelp();
		return 0;
	}

	//url = "rtsp://50.23.169.211/multicaststream_ch1_stream1";
	if (url == "")
		url = "rtsp://10.5.176.65/Streaming/Channels/1";
	if (!url.isEmpty())
		s.setupRtspClient(url);

	s.start();
#endif
	return a.exec();
}

