#include <QDir>
#include <QDirIterator>
#include <QApplication>

#include "moxadriver.h"
#include "ipstreamer.h"
#include "usbstreamer.h"
#include "version.h"
#include "yamgozstreamer.h"
#include "analogstreamer.h"
#include "algorithmmanager.h"
#include "algorithm/algorithmgrpcserver.h"
#include "applicationinfo.h"
#include "indevicetest.h"
#include "algorithmcontrolwidget.h"

#include <lmm/lmmcommon.h>

#include <ecl/debug.h>
#include <ecl/drivers/exarconfig.h>
#include <ecl/net/remotetcpconnection.h>
#include <ecl/drivers/qextserialport/qextserialport.h>

#include <unistd.h>
#include <execinfo.h>

/*
 * This function can be used to test Arya PT head
 * communication functionality
 */
static inline int serialTestForAryaHead()
{
	QextSerialPort *port = NULL;
	ExarConfig *exar = NULL;
	QString device = "/dev/ttyXRUSB1";
	port = new QextSerialPort(device, QextSerialPort::Polling);
	port->setBaudRate(BAUD19200);
	port->setDataBits(DATA_7);
	port->setStopBits(STOP_1);
	port->setParity(PAR_ODD);
	port->setFlowControl(FLOW_OFF);
	if (! port->open(QIODevice::ReadWrite)) {
		qDebug() << "Serial port not opened.";
		return -1;
	}
	exar = new ExarConfig(port->getFileDescriptor());
	exar->setPort("422");
	QString mes("<ZZZPTT:;ZZZ\?\?>");
	port->write(mes.toUtf8());
	sleep(1);
	QByteArray ba = port->readAll();
	qDebug() << "done" << ba.size() << ": ";// << QString::fromUtf8(ba);
	for (int i = 0; i < ba.size(); i++){
		qDebug("%d: 0x%x (%c)", i, (uchar)ba.at(i), ba.at(i));
	}
	QString str;
	str.append(QString::fromUtf8(ba));
	qDebug() << str;
	return 0;
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
#include "proto/AlgorithmCommunication.grpc.pb.h"
#include "proto/OrionCommunication.grpc.pb.h"

static int testOrionGrpc(const QString &action)
{
	QString ep = QString("127.0.0.1:50060");
	std::shared_ptr<grpc::Channel> chn = grpc::CreateChannel(ep.toStdString(), grpc::InsecureChannelCredentials());
	std::shared_ptr<OrionCommunication::OrionCommunicationService::Stub> stub = OrionCommunication::OrionCommunicationService::NewStub(chn);
	grpc::ClientContext ctx;
	::grpc::Status status;
	OrionCommunication::AppCommandResult res;
	if (action == "run_motion") {
		qDebug() << "running motion";
		OrionCommunication::DummyInfo req;
		status = stub->RunMotion(&ctx, req, &res);
	} else if (action == "stop_motion") {
		qDebug() << "stoping motion";
		OrionCommunication::DummyInfo req;
		status = stub->StopMotion(&ctx, req, &res);
	}
	qDebug() << status.error_code();
	return 0;
}

static int testGrpc(const QString &action)
{
	QString ep = QString("127.0.0.1:50059");
	std::shared_ptr<grpc::Channel> chn = grpc::CreateChannel(ep.toStdString(), grpc::InsecureChannelCredentials());
	std::shared_ptr<AlgorithmCommunication::AlgorithmService::Stub> stub = AlgorithmCommunication::AlgorithmService::NewStub(chn);
	grpc::ClientContext ctx;
	::grpc::Status status;
	if (action == "run_motion_roi") {
		qDebug() << "1";
		AlgorithmCommunication::RequestForAlgorithm req;
		req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm_Algorithm_MOTION);
		AlgorithmCommunication::MotionParameters *params = new AlgorithmCommunication::MotionParameters;
		AlgorithmCommunication::TRoi *Roi = new AlgorithmCommunication::TRoi;
		float points[16]{50,50,450,50,450,900,50,900,600,50,1800,50,1800,900,600,900};
		int polyCounter = 0;
		for (int i = 0; i < 2 ; ++i)
		{
			AlgorithmCommunication::TPolygon *Poly = Roi->add_polygon();
			Poly->set_is_active(true);
			for (int j = 0; j < 8; j += 2)
			{
				AlgorithmCommunication::TPoint *Point = Poly->add_points();
				Point->set_x(points[j + polyCounter]);
				Point->set_y(points[j + 1 + polyCounter]);
			}
			polyCounter = polyCounter + 4 * 2;
		}
		AlgorithmCommunication::TRectangle *myRect1 = Roi->mutable_rect1();
		AlgorithmCommunication::TRectangle *myRect2 = Roi->mutable_rect2();
		myRect1->mutable_upperleft()->set_x(100);
		myRect1->mutable_upperleft()->set_y(100);
		myRect1->mutable_bottomright()->set_x(200);
		myRect1->mutable_bottomright()->set_y(200);
		myRect2->mutable_upperleft()->set_x(300);
		myRect2->mutable_upperleft()->set_y(300);
		myRect2->mutable_bottomright()->set_x(500);
		myRect2->mutable_bottomright()->set_y(500);
		params->set_allocated_roilist(Roi);
		params->set_settingchoice(AlgorithmCommunication::MotionParameters_Settings_ROI);
		int a = qrand() % 100;
		params->set_sensitivity(a);
		req.set_allocated_motionparam(params);
		AlgorithmCommunication::ResponseOfRequests resp;
		status = stub->RunAlgorithm(&ctx, req, &resp);
	} if (action == "run_motion_sens") {
		AlgorithmCommunication::RequestForAlgorithm req;
		req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm_Algorithm_MOTION);
		AlgorithmCommunication::MotionParameters *params = new AlgorithmCommunication::MotionParameters;
		params->set_settingchoice(AlgorithmCommunication::MotionParameters_Settings_SENSITIVITY);
		int a = qrand() % 100;
		params->set_sensitivity(a);
		req.set_allocated_motionparam(params);
		AlgorithmCommunication::ResponseOfRequests resp;
		status = stub->RunAlgorithm(&ctx, req, &resp);
	} else if (action == "run_track"){
		AlgorithmCommunication::RequestForAlgorithm req;
		req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm_Algorithm_TRACKING);
		AlgorithmCommunication::TrackParameters *params = new AlgorithmCommunication::TrackParameters;
		params->set_tracktype(AlgorithmCommunication::TrackParameters_TrackType_AUTO);
		//		::AlgorithmCommunication::TrackObject* target;
		//		target->set_point_x(0.5);
		//		target->set_point_y(0.5);
		//		params->set_allocated_target(target);
		req.set_allocated_trackparam(params);
		AlgorithmCommunication::ResponseOfRequests resp;
		status = stub->RunAlgorithm(&ctx, req, &resp);
	} else if (action == "run_face") {
		AlgorithmCommunication::RequestForAlgorithm req;
		req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm_Algorithm_FACE_DETECTION);
		AlgorithmCommunication::FaceDetectionParameters *params = new AlgorithmCommunication::FaceDetectionParameters;
		AlgorithmCommunication::ResponseOfRequests resp;
		status = stub->RunAlgorithm(&ctx, req, &resp);
	} else if (action == "stop_motion") {
		AlgorithmCommunication::RequestForAlgorithm req;
		req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm_Algorithm_MOTION);
		AlgorithmCommunication::ResponseOfRequests resp;
		status = stub->StopAlgorithm(&ctx, req, &resp);
	} else if (action == "stop_track") {
		AlgorithmCommunication::RequestForAlgorithm req;
		req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm_Algorithm_TRACKING);
		AlgorithmCommunication::ResponseOfRequests resp;
		status = stub->StopAlgorithm(&ctx, req, &resp);
	} else if (action == "stop_face") {
		AlgorithmCommunication::RequestForAlgorithm req;
		req.set_algorithmtype(AlgorithmCommunication::RequestForAlgorithm_Algorithm_FACE_DETECTION);
		AlgorithmCommunication::ResponseOfRequests resp;
		status = stub->StopAlgorithm(&ctx, req, &resp);
	} else if (action == "alarm") {
		AlgorithmCommunication::AlarmReqInfo req;
		req.set_intervalmsecs(1000);
		auto stream = stub->GetAlarm(&ctx);
		if (!stream->Write(req)) {
			qDebug("Error starting getalarm request");
			return -1;
		}
		AlgorithmCommunication::Alarms res;
		while (stream->Read(&res)) {
			QDateTime dt = QDateTime::fromMSecsSinceEpoch(res.ts());
			qDebug("%d alarms at %s", res.alarms_size(), qPrintable(dt.toString()));
			for (int i = 0; i < res.alarms_size(); i++) {
				const AlgorithmCommunication::Alarm &alarm = res.alarms(i);
				qDebug("Alarm %d type is '%s'", i, alarm.type().data());
				if (alarm.type() == "cit") {
					const AlgorithmCommunication::CITInfo ci = alarm.cit();
					qDebug("module %d, pt %d, usb %d", ci.moduleinfo(), ci.motorinfo(), ci.usbinfo());
				}
			}

			req.set_intervalmsecs(1000);
			stream->Write(req);
		}
	}
	if (status.error_code() != grpc::OK) {
		qDebug("error '%d' in grpc call", status.error_code());
		return -1;
	}
	return 0;
}

#include "kardelenapi.h"

int kaapiClient(int argc, char *argv[])
{
	Q_UNUSED(argc);
	Q_UNUSED(argv);

	google::protobuf::Empty ereq;
	std::string ep = "127.0.0.1:50060";
	std::shared_ptr<grpc::Channel> chn = grpc::CreateChannel(ep, grpc::InsecureChannelCredentials());
	std::shared_ptr<kaapi::CameraService::Stub> stub = kaapi::CameraService::NewStub(chn);
	kaapi::PosInfo posi;
	grpc::ClientContext ctx;

	QString action = QString::fromUtf8(argv[1]);

	if (action == "position") {
		grpc::Status s = stub->GetPosition(&ctx, ereq, &posi);
		if (s.error_code() != grpc::StatusCode::OK) {
			fDebug("Some grpc error occurred");
			return -1;
		}
		fDebug("Got position: %lf %lf", posi.panpos(), posi.tiltpos());
	} else if (action == "gotopos") {
		QString panpos = QString::fromUtf8(argv[2]);
		QString tiltpos = QString::fromUtf8(argv[3]);
		kaapi::AbsoluteMoveResult res;
		kaapi::AbsoluteMoveParameters req;
		req.set_panpos(panpos.toFloat());
		req.set_tiltpos(tiltpos.toFloat());
		grpc::Status s = stub->MoveAbsolute(&ctx, req, &res);
		if (s.error_code() != grpc::StatusCode::OK) {
			fDebug("Some grpc error occurred");
			return -1;
		}
	} else if (action == "motroi") {
		ffDebug() << "motion roi";
		kaapi::MotionROIRequest req;
		kaapi::Rectangle *r = req.mutable_far();
		r->set_width(10);
		r->set_height(20);
		r = req.mutable_close();
		r->set_width(30);
		r->set_height(40);
		google::protobuf::Empty res;
		grpc::Status s = stub->SetMotionROI(&ctx, req, &res);
		if (s.error_code() != grpc::StatusCode::OK) {
			fDebug("Some grpc error occurred");
			return -1;
		}
	} else if (action == "status") {
		kaapi::CameraStatus cstatus;
		kaapi::GetCameraStatusParameters preq;
		grpc::Status s = stub->GetCameraStatus(&ctx, preq, &cstatus);
		if (s.error_code() != grpc::StatusCode::OK) {
			fDebug("Some grpc error occurred");
			return -1;
		}
		fDebug("capabilities = 0x%lx", cstatus.capabilities());
	} else if (action == "getnum") {
		QString index = QString::fromUtf8(argv[2]);
		kaapi::GetNumericParameterResponse resp;
		kaapi::GetNumericParameterRequest preq;
		preq.set_index(index.toInt());
		grpc::Status s = stub->GetNumericParameter(&ctx, preq, &resp);
		if (s.error_code() != grpc::StatusCode::OK) {
			fDebug("Some grpc error occurred");
			return -1;
		}
		fDebug("num param (%d): %lf", index.toInt(), resp.value().value());
	} else if (action == "getenum") {
		QString index = QString::fromUtf8(argv[2]);
		kaapi::GetEnumParameterResponse resp;
		kaapi::GetEnumParameterRequest preq;
		preq.set_index(index.toInt());
		grpc::Status s = stub->GetEnumParameter(&ctx, preq, &resp);
		if (s.error_code() != grpc::StatusCode::OK) {
			fDebug("Some grpc error occurred");
			return -1;
		}
		int32_t v = resp.value();
		fDebug("enum param (%d): %d", index.toInt(), v);
	} else if (action == "setcam") {
		QString cam = QString::fromUtf8(argv[2]);
		kaapi::SetCameraResponse resp;
		kaapi::SetCameraRequest preq;
		preq.set_cameratype(cam.toInt());
		grpc::Status s = stub->SetCamera(&ctx, preq, &resp);
		if (s.error_code() != grpc::StatusCode::OK) {
			fDebug("Some grpc error occurred");
			return -1;
		}
		fDebug("setcam (%d): 0x%lx", cam.toInt(), resp.capabilities());
	} else if (action == "setnum") {
		QString index = QString::fromUtf8(argv[2]);
		QString value = QString::fromUtf8(argv[3]);
		kaapi::SetNumericParameterResponse resp;
		kaapi::SetNumericParameterRequest preq;
		preq.set_index(index.toInt());
		kaapi::NumericParameter *np = preq.mutable_value();
		np->set_value(value.toDouble());
		grpc::Status s = stub->SetNumericParameter(&ctx, preq, &resp);
		if (s.error_code() != grpc::StatusCode::OK) {
			fDebug("Some grpc error occurred");
			return -1;
		}
		fDebug("set numeric parameter ok");
	} else if (action == "pan") {
		QString value = QString::fromUtf8(argv[2]);
		kaapi::SetNumericParameterResponse resp;
		kaapi::SetNumericParameterRequest preq;
		preq.set_index(kaapi::NUM_PARAM_YAW);
		kaapi::NumericParameter *np = preq.mutable_value();
		np->set_value(value.toDouble());
		np->set_byte2(0x0);
		grpc::Status s = stub->SetNumericParameter(&ctx, preq, &resp);
		if (s.error_code() != grpc::StatusCode::OK) {
			fDebug("Some grpc error occurred");
			return -1;
		}
		fDebug("pan ok");
	} else if (action == "tilt") {
		QString value = QString::fromUtf8(argv[2]);
		kaapi::SetNumericParameterResponse resp;
		kaapi::SetNumericParameterRequest preq;
		preq.set_index(kaapi::NUM_PARAM_PITCH);
		kaapi::NumericParameter *np = preq.mutable_value();
		np->set_value(value.toDouble());
		np->set_byte2(0x0);
		grpc::Status s = stub->SetNumericParameter(&ctx, preq, &resp);
		if (s.error_code() != grpc::StatusCode::OK) {
			fDebug("Some grpc error occurred");
			return -1;
		}
		fDebug("tilt ok");
	}

	return 0;
}

#undef DEBUG_H
#include "inc/debug.h"
static void init_smart_init()
{
	aselsmart::initSmartDebugging(3);
}

int main(int argc, char *argv[])
{
	if (QString::fromLatin1(argv[0]).contains("kaapic"))
		return kaapiClient(argc, argv);
	if (QString::fromLatin1(argv[0]).contains("apic"))
		return testGrpc(argv[1]);
	if (QString::fromLatin1(argv[0]).contains("orionc"))
		return testOrionGrpc(argv[1]);

	init_smart_init();
#if HAVE_TX1
	{
		extern char* get_libgpu_version();
		qDebug() << "libgpu.a version is" << get_libgpu_version();
	}
#else
	qDebug() << "libgpu.a version is unknown";
#endif

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

	ApplicationInfo *info = ApplicationInfo::instance();
	QCoreApplication *a;
	if (info->isGuiApplication())
		a = new QApplication(argc, argv);
	else
		a = new QGuiApplication(argc, argv);
	QDir::setCurrent(a->applicationDirPath());
	info->init();

	if (a->arguments().size() > 1) {
		if (a->arguments()[1] == "--version")
			qDebug() << VERSION_INFO;
		else if (a->arguments()[1] == "--lmm-version")
			qDebug() << LmmCommon::getLibraryVersion();
		else if (a->arguments()[1] == "--ecl-version")
			qDebug() << ecl::getLibraryVersion();
		return 0;
	}

	LmmCommon::init();
	ecl::initDebug();
	installSignalHandlers();

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
		qDebug() << "starting grpctest";
		return testGrpc(argv[1]);
	}

	return a->exec();
}
