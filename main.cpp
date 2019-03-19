#include <QDir>
#include <QApplication>

#include <execinfo.h>

#include <lmm/lmmcommon.h>

#include <ecl/debug.h>
#include <ecl/net/remotetcpconnection.h>

#include "smartstreamer.h"
#include "moxadriver.h"
#include "ipstreamer.h"
#include "usbstreamer.h"
#include "version.h"

#define GRPC_TEST 0
#define IP_STREAMER 0
#define USB_STREAMER 1
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
	if (signalNumber == SIGABRT) {
		qDebug("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! trying to continue");
		return;
	}
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
static inline QString getCommandlineParameter(const QString &par, QCoreApplication *a, const QString &desc, const QVariant &def)
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

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/channel.h>
#include <grpc++/create_channel.h>
#include <grpc++/client_context.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/credentials.h>
#include <grpc++/security/server_credentials.h>

#if GRPC_TEST
#include "proto/AlgorithmCommunication.grpc.pb.h"
static int testGrpc(const QString &action)
{
	QString ep = QString("127.0.0.1:50059");
	std::shared_ptr<grpc::Channel> chn = grpc::CreateChannel(ep.toStdString(), grpc::InsecureChannelCredentials());
	std::shared_ptr<AlgorithmCommunication::AlgorithmService::Stub> stub = AlgorithmCommunication::AlgorithmService::NewStub(chn);
	grpc::ClientContext ctx;

	::grpc::Status status;
	if (action == "run") {
		AlgorithmCommunication::RequestForAlgorithm req;
		req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm_Algorithm_MOTION);
		AlgorithmCommunication::MotionParameters *params = new AlgorithmCommunication::MotionParameters;
		params->set_settingchoice(AlgorithmCommunication::MotionParameters_Settings_SENSITIVITY);
		params->set_sensitivity(60);
		req.set_allocated_motionparam(params);
		AlgorithmCommunication::ResponseOfRequests resp;
		status = stub->RunAlgorithm(&ctx, req, &resp);
	} else if (action == "stop") {
		AlgorithmCommunication::RequestForAlgorithm req;
		req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm_Algorithm_MOTION);
		AlgorithmCommunication::ResponseOfRequests resp;
		status = stub->StopAlgorithm(&ctx, req, &resp);
	}

	if (status.error_code() != grpc::OK) {
		qDebug("error '%d' in grpc call", status.error_code());
		return -1;
	}

	qDebug() << "done grpc";

	return 0;
}
#endif

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QDir::setCurrent(a.applicationDirPath());

	if (a.arguments().size() > 1)
		if (a.arguments()[1] == "--version") {
			qDebug() << VERSION_INFO;
			return 0;
		}

	LmmCommon::init();
	ecl::initDebug();
	installSignalHandlers();

#if GRPC_TEST
	if (QString(argv[0]).contains("grpctest"))
		return testGrpc(argv[1]);
#endif

#if IP_STREAMER
	IpStreamer ipStr;
	ipStr.generatePipelineForOneSource(url);
	ipStr.start();
#endif

#if USB_STREAMER
	UsbStreamer usbStr;
	usbStr.generatePipelineForOneSource("/dev/video0");
	usbStr.start();
#endif

	return a.exec();
}

