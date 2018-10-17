#include <QDir>
#include <QApplication>

#include <execinfo.h>

#include <lmm/lmmcommon.h>

#include <ecl/debug.h>
#include <ecl/net/remotetcpconnection.h>

#include "smartstreamer.h"
#include "moxadriver.h"

static void printStackTrace(void)
{
	void *array[25];
	size_t size;
	char **strings;
	size_t i;

	size = backtrace (array, 25);
	strings = backtrace_symbols (array, size);

	qWarning("Obtained %zd stack frames.", size);

	for (i = 0; i < size; i++)
		qWarning("%s", strings[i]);

	free (strings);
}

static void signalHandler(int signalNumber)
{
	static int once = 0;
	if (once) {
		exit(23);
		return;
	}
	once = 1;
	Qt::HANDLE id = QThread::currentThreadId();
	qWarning("crash: Received signal %d, thread id is %p", signalNumber, id);
	printStackTrace();

	exit(19);
}

static int installSignalHandlers()
{
	struct sigaction sigInstaller;
	sigset_t block_mask;

	sigemptyset(&block_mask);
	sigfillset(&block_mask);
	sigInstaller.sa_flags   = (SA_RESTART) ;
	sigInstaller.sa_mask    = block_mask;
	sigInstaller.sa_handler = &signalHandler;
	sigaction(SIGSEGV, &sigInstaller, NULL);
	sigaction(SIGINT, &sigInstaller, NULL);
	sigaction(SIGTERM, &sigInstaller, NULL);
	sigaction(SIGABRT, &sigInstaller, NULL);
	sigaction(SIGPIPE, &sigInstaller, NULL);
	sigaction(SIGBUS, &sigInstaller, NULL);
	sigaction(SIGFPE, &sigInstaller, NULL);

	return 0;
}

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

	installSignalHandlers();

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
	getCmdParStr(pars.ptzUrl, "--ptz-url", "System head remote target, ekinoks 'eth;10.5.20.92:8998', arya '50.23.169.213'");
	getCmdParInt(pars.offline, "--offline", "This mode just use rtp, rtsp streaming flows.");
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

	if (!pars.offline)
		s.setupVideoAnalysis();

	if (!pars.ptzUrl.isEmpty())
		s.setupPanTiltZoomDriver(pars.ptzUrl);

	if (!url.isEmpty())
		s.setupRtspClient(url);

	s.start();
#endif
	return a.exec();
}

