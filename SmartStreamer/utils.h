#ifndef UTILS_H
#define UTILS_H
#include <QFile>
#include <QThread>
#include <QJsonDocument>
#include <signal.h>
#include <execinfo.h>

namespace utils {
static void exportConfigs() {
	if (!QFile::exists("/etc/smartstreamer/smartconfig.json") && QFile::exists("/etc/aselsan_platform")) {
		QFile f("/etc/aselsan_platform");
		f.open(QIODevice::ReadOnly);
		QString plat = QString::fromUtf8(f.readAll()).trimmed();
		f.close();
		qDebug("Auto-generating smartconfig for device '%s'", qPrintable(plat));
		f.setFileName(":/data/platforms.json");
		f.open(QIODevice::ReadOnly);
		QJsonObject obj = QJsonDocument::fromJson(f.readAll()).object();
		f.close();
		if (obj.contains(plat)) {
			qDebug("Found json config for platform");
			QJsonObject config = obj[plat].toObject();
			QJsonDocument doc(config);
			f.setFileName("/etc/smartstreamer/smartconfig.json");
			if (!f.open(QIODevice::WriteOnly)) {
				qDebug("Cannot open %s for writing", qPrintable(f.fileName()));
			} else {
				f.write(doc.toJson());
				f.close();
			}
		}
	}
}

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

} // utils

#endif // UTILS_H
