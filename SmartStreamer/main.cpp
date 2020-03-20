#include <QDir>
#include <QDirIterator>
#include <QApplication>
#include <QJsonObject>
#include <QJsonDocument>

#include "debug.h"
#include "utils.h"
#include "version.h"
#include "indevicetest.h"
#include "applicationinfo.h"
#include "algorithmcontrolwidget.h"
#include "smartstreamertools.h"

#include <lmm/lmmcommon.h>
#include <lmm/players/basestreamer.h>

#include <unistd.h>
#include <execinfo.h>

#undef DEBUG_H
#include "libsmart/debug.h"
static void init_smart_init()
{
	aselsmart::initSmartDebugging(3);
}

int main(int argc, char *argv[])
{
	if (QString::fromLatin1(argv[0]).contains("SmartStreamerTools")) {
		SmartStreamerTools sst;
		return sst.toolsMain(argc, argv);
	}
#if HAVE_TX1
	init_smart_init();
	{
		extern char* get_libgpu_version();
		qDebug() << "libgpu.a version is" << get_libgpu_version();
	}
#else
	qDebug() << "libgpu.a version is unknown";
#endif
	utils::exportConfigs();
	ApplicationInfo *info = ApplicationInfo::instance();
	QCoreApplication *a;
	if (info->isGuiApplication())
		a = new QApplication(argc, argv);
	else
		a = new QGuiApplication(argc, argv);
	if (a->arguments().size() > 1) {
		if (a->arguments()[1] == "--version")
			qDebug() << VERSION_INFO;
		else if (a->arguments()[1] == "--lmm-version")
			qDebug() << LmmCommon::getLibraryVersion();
		else if (a->arguments()[1] == "--ecl-version")
			qDebug() << ecl::getLibraryVersion();
		return 0;
	}

	QDir::setCurrent(a->applicationDirPath());
	info->init();

	LmmCommon::init();
	ecl::initDebug();
	utils::installSignalHandlers();

	info->checkStartupDelay();

	/* config export */
	QDir d("/etc/");
	d.mkdir("smartstreamer");
	d = QDir("/");
	QDirIterator it (":", QDirIterator::Subdirectories);
	while (it.hasNext()) {
		QString s = it.next();
		if (!s.startsWith(":/data/"))
			continue;
		QFileInfo finfo(s);
		QString dst = finfo.absoluteFilePath().replace(":/data/", "/etc/smartstreamer/");
		if (finfo.suffix().isEmpty())
			d.mkpath(dst);
		else if (!QFile::exists(dst)) {
			qDebug() << "exporting" << dst << QFile::copy(finfo.absoluteFilePath(), dst);
			QFile::setPermissions(dst, QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther);
		}
	}

	if (!QFile::exists("alg_parameters.txt"))
		QDir::setCurrent("/etc/smartstreamer/" + info->algorithmSet());
	info->startPtzpDriver();

	BaseStreamer *streamer = info->createAppStreamer();
	if (streamer) {
		streamer->start();
		if (info->getIDT())
			info->getIDT()->addPipeline("video_source0", streamer->getPipeline(0));
	} else {
		qDebug() << "no suitable streamer found for your config";
		qDebug() << "Closing...";
		return 0;
	}

	return a->exec();
}
