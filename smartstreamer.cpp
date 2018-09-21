#include "smartstreamer.h"
#include "mjpegserver.h"
#include "seiinserter.h"

#include <lmm/debug.h>
#include <lmm/bufferqueue.h>
#include <lmm/videoscaler.h>
#include <lmm/qtvideooutput.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/rtp/rtpreceiver.h>
#include <lmm/rtsp/rtspclient.h>
#include <lmm/tools/unittimestat.h>
#include <lmm/rtp/rtptransmitter.h>
#include <lmm/rtsp/basertspserver.h>
#include <lmm/ffmpeg/ffmpegdecoder.h>
#include <lmm/ffmpeg/ffmpegcolorspace.h>
#include <lmm/pipeline/functionpipeelement.h>

#include <ecl/ptzp/ptzphead.h>
#include <ecl/ptzp/aryadriver.h>

#ifdef HAVE_VIA_WRAPPER
#include <ViaWrapper/viawrapper.h>
#endif

extern "C" {
#include <libavformat/avformat.h>
}

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/channel.h>
#include <grpc++/create_channel.h>
#include <grpc++/client_context.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/credentials.h>
#include <grpc++/security/server_credentials.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::Status;
using namespace std;

#include <QFile>
#include <QImage>
#include <QBuffer>
#include <QProcess>
#include <unistd.h>

#define PRINT_BUFS 0
#define PRINT_CUDA 1

class GrpcThread : public QThread
{
public:
	GrpcThread (quint16 port, SmartStreamer *s)
	{
		servicePort = port;
		streamer = s;
	}
	void run()
	{
		string ep(qPrintable(QString("0.0.0.0:%1").arg(servicePort)));
		ServerBuilder builder;
		builder.AddListeningPort(ep, grpc::InsecureServerCredentials());
		builder.RegisterService(streamer);
		std::unique_ptr<Server> server(builder.BuildAndStart());
		server->Wait();
	}

protected:
	int servicePort;
	SmartStreamer *streamer;
};

SmartStreamer::SmartStreamer(QObject *parent)
	: BaseStreamer(parent), OrionCommunication::AppConfig::Service()
{
	arya = new AryaDriver();
	arya->startSocketApi(8945);
	arya->setTarget("50.23.169.213");
	arya->startGrpcApi(50058);
	pt = arya->getHead(0);
	grpcServ = new GrpcThread(50059, this);
	grpcServ->start();
#ifdef HAVE_VIA_WRAPPER
	wrap = new ViaWrapper();
#endif
}

bool SmartStreamer::goToZeroPosition()
{
	float span = 360.0;
	span = pt->getPanAngle();
	QElapsedTimer elap;
	elap.start();
	while (span < 359.89 && span > 0.1) {
		pt->panTiltGoPos(0, 0);
		sleep(3);
		span = (int) pt->getPanAngle();
		if (elap.elapsed() > 30000) {
			mDebug("I can't found  0,0 position on this device.. ");
			return false;
		}
	}
	if (wrap)
		wrap->panaroma.initStart = false;
	return true;
}

bool SmartStreamer::startSpinnig(float sSpeed)
{
	if (sSpeed == 0)
		sSpeed = 0.225;
	pt->panTiltAbs(sSpeed, 0);
	int span = 0;
	span = (int) pt->getPanAngle();
	while (span == 0)
		span = (int) pt->getPanAngle();
	if (span != 0) {
		wrap->panaroma.startSpinnig = true;
		wrap->panaroma.initializing = 1;
	}
	return true;
}

void SmartStreamer::doPanaroma(const RawBuffer &buf)
{
	if (wrap->panaroma.stop)
		return;
	if (wrap->panaroma.start) {
		if (wrap->panaroma.initStart)
			if (!goToZeroPosition())
				mDebug("Closing panaroma mode");
		if (!wrap->panaroma.startSpinnig)
			startSpinnig();
		float pan_tilt_zoom_read[3];
		float pan, tilt; int zoom;
		zoom = pt->getZoom();
		pan = pt->getPanAngle();
		tilt = pt->getTiltAngle();
		mInfo("Panaroma doing, pan angle %f, Tilt angle %f, Zoom angle %d", pan, tilt, zoom);
		pan_tilt_zoom_read[0] = (float) pan / 17777.777;
		pan_tilt_zoom_read[1] = (float) tilt / 17777.777;
		pan_tilt_zoom_read[2] = zoom;
		wrap->viaPan(buf, pan_tilt_zoom_read, wrap->panaroma.initializing);
		wrap->panaroma.initializing = 0;
		if (wrap->meta[0] != 0) {
			pt->panTiltStop();
			wrap->stopPanaroma();
		}
	}
}

void SmartStreamer::doMotionDetection(const RawBuffer &buf)
{
	if (wrap->motion.stop)
		return;
	if (wrap->motion.start) {
		wrap->viaBase(buf, wrap->motion.initializing);
		wrap->motion.initializing = 0;
		if (sei) {
			QByteArray ba = QByteArray((char *)wrap->meta, 4096);
			sei->processMessage(ba);
		}
	}
}

int SmartStreamer::setupRtspClient(const QString &rtspUrl)
{
	printParameters();

	rtp = new RtpReceiver(this);
	rtpout = new RtpTransmitter(this);
	rtpout->setH264SEIInsertion(true);
	rtpout->useIncomingTimestamp(false);

	/* queues will be used to de-multiplex streams */
	BufferQueue *queue = new BufferQueue;
	BufferQueue *queueScalerEngine = new BufferQueue;

	vout = new QtVideoOutput;
	dec = new FFmpegDecoder;
	dec->setBufferCount(pars.decBufferCount);
	if (pars.decWidth)
		dec->setVideoResolution(pars.decWidth, pars.decHeight); //352x290
	VideoScaler *rgbConv2 = new VideoScaler;
	rgbConv2->setMode(1);
	VideoScaler *rgbScaler = new VideoScaler;
	rgbScaler->setOutputResolution(pars.rgbMainWidth, pars.rgbMainHeight);
	VideoScaler *yuvScaler = new VideoScaler;
	yuvScaler->setOutputResolution(pars.secWidth, pars.secHeight);

	BaseLmmPipeline *p1 = addPipeline();
	p1->append(rtp);
	p1->append(queue);
	p1->append(dec);
	//p1->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::checkPoint));
	p1->append(queueScalerEngine);
	if (pars.pipelineFlags & Parameters::EL_YUV_PROCESS)
		p1->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::processMainYUV));
	p1->end();
	if (pars.rtpBufferDuration) {
		rtp->getOutputQueue(0)->setBufferDuration(pars.rtpBufferDuration);
		rtp->getOutputQueue(0)->setTimestampingMethod(ElementIOQueue::TS_DURATION);
	}
	if (pars.decOutputInFps > 0 && pars.decOutputOutFps > 0) {
		dec->getOutputQueue(0)->setRateReduction(pars.decOutputInFps, pars.decOutputOutFps);
	}

	sei = new SeiInserter;
	sei->setAlarmTemplate("sei_alarm_template.xml");
	if (pars.pipelineFlags & Parameters::EL_RTP_OUTPUT) {
		BaseLmmPipeline *p2 = addPipeline();
		p2->append(queue);
		p2->append(sei);
		p2->append(rtpout);
		//p2->append(wss);
		p2->end();
	}

	MjpegElement *mjpeg = new MjpegElement(4571);
	BaseLmmPipeline *p3 = addPipeline();
	p3->append(queueScalerEngine);
	p3->append(rgbConv2);
	if (pars.pipelineFlags & Parameters::EL_RGB_PROCESS)
		p3->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::processMainRGB));
	if (pars.pipelineFlags & Parameters::EL_QT_VOUT)
		p3->append(vout);
	p3->append(rgbScaler);
	if (pars.pipelineFlags & Parameters::EL_RGBS_PROCESS)
		p3->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::processScaledRGB));
	if (pars.pipelineFlags & Parameters::EL_MJPEG_OUTPUT)
		p3->append(mjpeg);
	p3->end();

	if (pars.secWidth && pars.secHeight) {
		BaseLmmPipeline *p4 = addPipeline();
		p4->append(queueScalerEngine);
		p4->append(yuvScaler);
		if (pars.pipelineFlags & Parameters::EL_YUVS_PROCESS)
			p4->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::processScaledYUV));
		p4->end();
	}

	rtsp = new RtspClient(this);
	rtsp->addSetupTrack("videoTrack", rtp);
	rtsp->addSetupTrack("trackID=1", rtp);
	rtsp->addSetupTrack("video", rtp);
	if (!pars.rtspClientUser.isEmpty())
		rtsp->setAuthCredentials(pars.rtspClientUser, pars.rtspClientPass);//"admin", "moxamoxa"
	if (rtspUrl.startsWith("rtsp://"))
		rtsp->setServerUrl(QString("%1").arg(rtspUrl));
	else
		rtsp->setServerUrl(QString("rtsp://%1").arg(rtspUrl));
	if (pars.enableMoxaHacks)
		rtsp->setMoxaHacks(true);

	rtspServer = new BaseRtspServer(this, 8554);
	rtspServer->addStream("stream1", false, rtpout);
	rtspServer->addStream("stream1m",true, rtpout, 15678);
	rtspServer->addMedia2Stream("videoTrack", "stream1", false, rtpout);
	rtspServer->addMedia2Stream("videoTrack", "stream1m", true, rtpout);
	if (!pars.rtspServerUser.isEmpty()) {
		rtspServer->setRtspAuthentication(BaseRtspServer::AUTH_SIMPLE);
		rtspServer->setRtspAuthenticationCredentials(pars.rtspServerUser, pars.rtspServerPass);
	}

	return 0;
}

int SmartStreamer::processMainYUV(const RawBuffer &buf)
{
	if (PRINT_BUFS)
		ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
				  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
#ifdef HAVE_VIA_WRAPPER
	doPanaroma(buf);
	doMotionDetection(buf);
#endif
	return 0;
}

int SmartStreamer::processMainRGB(const RawBuffer &buf)
{
	if (PRINT_BUFS)
		ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
				  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
	mutex.lock();
	screenMainShot = doScreenShot(buf);
	mutex.unlock();
	return 0;
}

int SmartStreamer::processScaledRGB(const RawBuffer &buf)
{
	if (PRINT_BUFS)
		ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
				  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
	mutex.lock();
	screenSecShot = doScreenShot(buf);
	mutex.unlock();
	return 0;
}

int SmartStreamer::processScaledYUV(const RawBuffer &buf)
{
	if (PRINT_BUFS)
		ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
				  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
	return 0;
}

int SmartStreamer::checkPoint(const RawBuffer &buf)
{
	Q_UNUSED(buf);
	static UnitTimeStat stat;
	stat.addStat();
	ffDebug() << stat.min << stat.max << stat.avg;
	return 0;
}

QByteArray SmartStreamer::doScreenShot(const RawBuffer &buf)
{
	QByteArray ba;
	QImage im;
	if (buf.size() == buf.constPars()->videoWidth * buf.constPars()->videoHeight * 3)
		im = QImage((const uchar *)buf.constData(), buf.constPars()->videoWidth, buf.constPars()->videoHeight,
					QImage::Format_RGB888);
	else
		im = QImage((const uchar *)buf.constData(), buf.constPars()->videoWidth, buf.constPars()->videoHeight,
					QImage::Format_RGB32);
	QBuffer qbuf(&ba);
	qbuf.open(QIODevice::WriteOnly);
	im.save(&qbuf, "JPG");
	return ba;
}

grpc::Status SmartStreamer::SetCurrentMode(grpc::ServerContext *context, const OrionCommunication::SetModeQ *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	Q_UNUSED(response)
	// unusing function.
	return grpc::Status::OK;
}

grpc::Status SmartStreamer::GetCurrentMode(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::SetModeQ *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)

	ViaWrapper::ModeState myMode = wrap->getMode();

	if (ViaWrapper::Panaroma == myMode)
		response->set_mode(response->PANAROMA);
	else if (ViaWrapper::Motion == myMode)
		response->set_mode(response->MOTION_DETECTION);
	else
		response->set_mode(response->NONE);
	return grpc::Status::OK;
}

grpc::Status SmartStreamer::SetPanaromaParameters(grpc::ServerContext *context, const OrionCommunication::PanoramaPars *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(response)
	qDebug() << request->fov();
	return grpc::Status::OK;
}

grpc::Status SmartStreamer::GetPanaromaFrames(grpc::ServerContext *context, const OrionCommunication::GetFrames *request, ::grpc::ServerWriter<OrionCommunication::PanoramaFrame> *writer)
{
	Q_UNUSED(context)
	if (wrap->getMode() != ViaWrapper::Panaroma)
		return Status::CANCELLED;
	bool lastFrame;
	if (request->modeframe() == 0)
		lastFrame = false;
	else lastFrame = true;
	QString next = "/home/ubuntu/Desktop/Pan_images/Pan%1.jpg";
	QString nextFinal = "/home/ubuntu/Desktop/Pan_images/PanFinal.jpg";
	if (lastFrame) {
		while (1) {
			QString picture;
			picture = nextFinal;
			if (!QFile::exists(picture)) {
				sleep(1);
				continue;
			}
			sleep(2); // writing image  to folder, wait for buffer
			QByteArray imageBuf = convertImageToByteArray(picture);
			OrionCommunication::PanoramaFrame panFrames;
			panFrames.set_valid(true);
			panFrames.set_progress(100);
			panFrames.set_framedata(imageBuf.data(), imageBuf.size());
			writer->Write(panFrames);
			return Status::OK;
		}
	} else {
		int progress = 0;
		int nameLastValue = 200;
		QElapsedTimer elaps;
		elaps.start();
		while (1) {
			if (elaps.elapsed() > 60000 * 4) // maybe we can see connection error or panaroma cancel state..
				return Status::CANCELLED;
			QString picture;
			picture = next.arg(QString::number(nameLastValue), 4, QChar('0'));
			if (QFile::exists(nextFinal)) {
				picture = nextFinal;
				progress = 9;
			} else if (!QFile::exists(picture)) {
				sleep(1);
				continue;
			}
			sleep(1);
			QByteArray imageBuf = convertImageToByteArray(picture);
			OrionCommunication::PanoramaFrame panFrames;
			panFrames.set_valid(true);
			progress = progress + 1;
			panFrames.set_progress(progress * 10);
			panFrames.set_framedata(imageBuf.data(), imageBuf.size());
			writer->Write(panFrames);
			nameLastValue = nameLastValue + 200;
			if (picture == nextFinal)
				break;
		}
	}
	return Status::OK;
}

grpc::Status SmartStreamer::RunPanaroma(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	Q_UNUSED(response)
	QProcess::execute("bash -c \"rm -f /home/ubuntu/Desktop/Pan_images/*\"");
	wrap->startPanaroma();
	return Status::OK;
}

grpc::Status SmartStreamer::RunMotion(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	wrap->startMotion();
	response->set_err(0);
	return Status::OK;
}

grpc::Status SmartStreamer::StopPanaroma(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	wrap->stopPanaroma();
	pt->panTiltStop();
	response->set_err(0);
	return Status::OK;
}

grpc::Status SmartStreamer::StopMotion(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	wrap->stopMotion();
	response->set_err(0);
	return Status::OK;
}

grpc::Status SmartStreamer::GetSecScreenShot(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::ScreenFrame *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	Q_UNUSED(response)
	if (screenSecShot.isEmpty())
		return Status::CANCELLED;
	mutex.lock();
	response->set_frame(screenSecShot, screenSecShot.size());
	mutex.unlock();
	return Status::OK;
}

grpc::Status SmartStreamer::GetMainScreenShot(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::ScreenFrame *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	Q_UNUSED(response)
	if (screenMainShot.isEmpty())
		return Status::CANCELLED;
	mutex.lock();
	response->set_frame(screenMainShot, screenMainShot.size());
	mutex.unlock();
	return Status::OK;
}

grpc::Status SmartStreamer::SetMotionDetectionParameters(grpc::ServerContext *context, const OrionCommunication::TRoi *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(response)
	qDebug() << "polygon byte size " << request->polygon().size();
	if (request->polygon().size() < 1) {
		response->set_err(1);
		return grpc::Status::CANCELLED;
	}
	QFile f("points.txt");
	if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
		response->set_err(2);
		return Status::CANCELLED;
	}
	QTextStream out(&f);

	out << request->polygon().size() << "\n";
	for (int i = 0; i < request->polygon().size(); i++) {
		OrionCommunication::TPolygon myPoly = request->polygon(i);
		out << myPoly.points_size() << "\n";
	}

	for (int i = 0; i < request->polygon().size(); i++) {
		OrionCommunication::TPolygon myPoly = request->polygon(i);
		qDebug() << "Is polygon active? " << myPoly.is_active();
		int state = (myPoly.is_active() == true) ? 1 : 0;
		out << state << "\n";
	}

	for (int i = 0; i < request->polygon().size(); i++) {
		OrionCommunication::TPolygon myPoly = request->polygon(i);
		for (int k = 0; k < myPoly.points_size(); k++) {
			qDebug() << myPoly.points(k).x();
			qDebug() << myPoly.points(k).y();
			out << myPoly.points(k).x() << "\n";
			out << myPoly.points(k).y() << "\n";
		}
	}

	// rect1
	out << request->rect1().upperleft().x() << "\n";
	out << request->rect1().upperleft().y() << "\n";
	out << request->rect1().bottomright().x() << "\n";
	out << request->rect1().bottomright().y() << "\n";

	// rect2
	out << request->rect2().upperleft().x() << "\n";
	out << request->rect2().upperleft().y() << "\n";
	out << request->rect2().bottomright().x() << "\n";
	out << request->rect2().bottomright().y() << "\n";
	f.close();
	return Status::OK;
}

grpc::Status SmartStreamer::GetMotionDetectionParameters(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::TRoi *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	QFile f("points.txt");
	if (!f.open(QIODevice::ReadOnly))
		return grpc::Status::CANCELLED;
	QTextStream in(&f);
	QStringList lines;
	while (!in.atEnd()) {
		lines << in.readLine();
	}
	int numOfPolySize = lines.at(0).toInt();
	QList<int> pointSize;
	for (int i = 0; i < numOfPolySize; i++) {
		pointSize << lines.at(i + 1).toInt();
	}

	QList<int> polyState;
	for (int i = 0; i < numOfPolySize; i++) {
		polyState << lines.at(i + 1 + numOfPolySize).toInt();
	}

	int polyCounter = 0;
	for (int j = 0; j < numOfPolySize; j++) {
		OrionCommunication::TPolygon *myPoly1 = response->add_polygon();
		bool state = (polyState[j] == 1) ? true : false;
		myPoly1->set_is_active(state);
		for (int i = 0; i < pointSize[j] * 2; i = i + 2) {
			OrionCommunication::TPoint *myPoints = myPoly1->add_points();
			myPoints->set_x(lines.at(polyCounter + i + 1  + 2 * numOfPolySize).toFloat());
			myPoints->set_y(lines.at(polyCounter + i + 2  + 2 * numOfPolySize).toFloat());
		}
		polyCounter = polyCounter + 2 * pointSize[j];
	}

	OrionCommunication::TRectangle *myRect1 = response->mutable_rect1();
	OrionCommunication::TRectangle *myRect2 = response->mutable_rect2();
	myRect1->mutable_upperleft()->set_x(lines.at(lines.size() - 8).toFloat());
	myRect1->mutable_upperleft()->set_y(lines.at(lines.size() - 7).toFloat());
	myRect1->mutable_bottomright()->set_x(lines.at(lines.size() - 6).toFloat());
	myRect1->mutable_bottomright()->set_y(lines.at(lines.size() - 5).toFloat());

	myRect2->mutable_upperleft()->set_x(lines.at(lines.size() - 4).toFloat());
	myRect2->mutable_upperleft()->set_y(lines.at(lines.size() - 3).toFloat());
	myRect2->mutable_bottomright()->set_x(lines.at(lines.size() - 2).toFloat());
	myRect2->mutable_bottomright()->set_y(lines.at(lines.size() - 1).toFloat());
	return Status::OK;
}

grpc::Status SmartStreamer::GetSensivityParameter(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::SetSensivity *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	response->set_sensivity(wrap->motion.sensivity);
	return Status::OK;
}

grpc::Status SmartStreamer::SetSensivityParameter(grpc::ServerContext *context, const OrionCommunication::SetSensivity *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(response)

	wrap->motion.sensivity = request->sensivity();
	response->set_err(0);
	return Status::OK;
}

grpc::Status SmartStreamer::GotoPanaromaPixel(grpc::ServerContext *context, const OrionCommunication::TPoint *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	Q_UNUSED(response)
	float point_x = (float)((int)((request->x() * 360 + 1) * 17777));
	float theta = 6.5 * 576.0 / 720.0;
	float point_y = - (((float)request->y() - 0.5) / 0.5 * theta / 2.0 * 17777.77);
	pt->panTiltGoPos(point_x, point_y);

	return Status::OK;
}

void SmartStreamer::timeout()
{
	if (PRINT_CUDA) {
		ffDebug() << "Motion State: " << "Stop state = " <<  wrap->motion.stop  << "Start state = " << wrap->motion.start;
		ffDebug() << "Panaroma state: " << "Stop state = " <<  wrap->panaroma.stop  << "Start state = " << wrap->panaroma.start;
	}
	PipelineManager::timeout();
}

#define printPar(member_name) lines << QString("\t%1: %2").arg(#member_name).arg(pars.member_name)
#define printParHex(member_name) lines << QString("\t%1: 0x%2").arg(#member_name).arg(pars.member_name, 8, 16, QChar('0'))
void SmartStreamer::printParameters()
{
	qDebug("Starting with following parameters:");
	QStringList lines;
	printPar(decWidth);
	printPar(decHeight);
	printParHex(pipelineFlags);
	printPar(secWidth);
	printPar(secHeight);
	printPar(rgbMainWidth);
	printPar(rgbMainHeight);
	printPar(decBufferCount);
	printPar(decOutputInFps);
	printPar(decOutputOutFps);
	printPar(rtpBufferDuration);
	printPar(enableMoxaHacks);
	printPar(rtspClientUser);
	printPar(rtspClientPass);
	printPar(rtspServerUser);
	printPar(rtspServerPass);
	foreach (QString line, lines)
		qDebug("%s", qPrintable(line));
}

int SmartStreamer::pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf)
{
	mInfo("New pipeline %p data: %s - %d bytes", p, qPrintable(buf.getMimeType()), buf.size());
	if (PRINT_BUFS && p == getPipeline(0))
		qDebug() << buf.size() << buf.getMimeType();

	return 0;
}

QByteArray SmartStreamer::convertImageToByteArray(const QString &filename)
{
	QImage im(filename);
	QByteArray ba;
	QBuffer qbuf(&ba);
	qbuf.open(QIODevice::WriteOnly);
	im.save(&qbuf, "JPG");
	return ba;
}
