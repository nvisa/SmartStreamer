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

#ifdef HAVE_VIA_WRAPPER
#include <ViaWrapper/viawrapper.h>
#endif

extern "C" {
	#include <libavformat/avformat.h>
}

#define PRINT_BUFS 0

#define TEST 0 //0 base 1 panaroma

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/channel.h>
#include <grpc++/create_channel.h>
#include <grpc++/client_context.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/credentials.h>
#include <grpc++/security/server_credentials.h>

#include <QFile>
#include <QImage>
#include <QBuffer>
#include <QProcess>
#include <unistd.h>
#include <proto/camback.grpc.pb.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::Status;

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using namespace std;
using camback::PTZService;
using camback::PTZInfoQ;
using camback::PTZCmdInfo;
using camback::PtzCommandResult;

class GrpcPTZClient
{
public:
    explicit GrpcPTZClient(std::shared_ptr<grpc::Channel> chn)
    {
//      std::shared_ptr<grpc::Channel> chn = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
        stub = camback::PTZService::NewStub(chn);
    }

    int setPanAbsCmd(const QString &cmd)
    {
        camback::PanCmdInfo req;
        req.set_pan_cmd(cmd.toLatin1().data(), cmd.size());
        grpc::ClientContext ctx;
        camback::PtzCommandResult res;
        stub->PanAbsCmd(&ctx, req, &res);
        return 0;
    }
    int setPanTiltPos(float pan_pos, float tilt_pos)
    {
        camback::PanTiltPos req;
        req.set_pan_pos(pan_pos);
        req.set_tilt_pos(tilt_pos);
        grpc::ClientContext ctx;
        camback::PtzCommandResult res;
        stub->SetPanTiltPos(&ctx, req, &res);
        return 0;
    }
    int setPanTiltAbs(float pan_speed, float tilt_speed)
    {
        camback::PtzCmdPar req;
        req.set_pan_abs(pan_speed);
        req.set_tilt_abs(tilt_speed);
        grpc::ClientContext ctx;
        camback::PtzCommandResult res;
        stub->PanTiltAbs(&ctx, req, &res);
        return 0;
    }
    int  getPTZPosInfo(int &pan, int &tilt, int &zoom)
    {
        camback::PTZInfoQ req;
        grpc::ClientContext ctx;
        camback::PTZPosInfo res;
        stub->GetPTZPosInfo(&ctx, req, &res);
        pan = res.pan_pos();
        tilt = res.tilt_pos();
        zoom = res.zoom_pos();
        return 0;
    }

private:
    std::unique_ptr<camback::PTZService::Stub> stub;
};

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

class CudaConfigurations
{
public:
    enum CudaList {
        CU_NONE,
        CU_PAN_MODE,
        CU_MOT_MODE,
        CU_REC_MODE,
        CU_BYPASS_MODE
    };

    CudaConfigurations()
    {
        activeMode = 0;
    }

    int getActiveMode()
    {
        return activeMode;
    }

    void changeMode(CudaList cuMode)
    {
        l.lock();
        activeMode = cuMode;
        l.unlock();
        return;
    }

private:
    QMutex l;
    int activeMode;
};

SmartStreamer::SmartStreamer(QObject *parent)
    : BaseStreamer(parent), OrionCommunication::AppConfig::Service()
{
    grpcServ = new GrpcThread(50054, this);
    grpcServ->start();
    cuConf = new CudaConfigurations();
    ptzclient = new GrpcPTZClient(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
#ifdef HAVE_VIA_WRAPPER
	wrap = new ViaWrapper();
//    wrap->pan_enabled = false;
    socket = new QTcpSocket();
    panaroma = false;
    QProcess::execute("sh -c \"rm -f /home/ubuntu/Desktop/Pan_images/*\"");
    panInitOnce = false;
    baseInitOnce = false;
    initAlgoritmOnce = false;
    initBaseAlgoritmOnce = false;
    panStop = false;
    base = false;
    baseStop = false;
    if (TEST == 0) {
        base = true;
    }
    if (TEST == 1) {
        panaroma = true;
    }
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(switchEvent()));
    timer->start(1000);
#endif
}

void SmartStreamer::switchEvent()
{
    qDebug() << "base " << base << "panaroma" << panaroma;
    if (panaroma && !panInitOnce) {
        panStop = false;
        panInit = true;
        panMotionStart = false;
        panTiltInfo = false;
        panaromaCounter = 0;
        connected();
        panInitOnce = true;
        speed = "90000";
        initPanInViaBa = 1;
    } else if (base && !baseInitOnce) {
//        QTimer::singleShot(20000, this, SLOT(connected()));
        initBaseInViaBa = 1;
        baseInitOnce = true;
        connected();
    }
}

void SmartStreamer::initPanaroma()
{
    panStop = false;
    panInit = true;
    panMotionStart = false;
    panTiltInfo = false;
    panaromaCounter = 0;
    panInitOnce = true;
    speed = "90000";
    initPanInViaBa = 1;
}

void SmartStreamer::GoToPositionPanaroma()
{
    if (panInit) {
            ptzclient->setPanTiltPos(0,0);
        while(panaromaCounter != 100) {
            qDebug() << "going to position";
            panaromaCounter++;
            panMotionStart = true;
            panInit = false;
        }
    }
}

void SmartStreamer::connected()
{
    static int cnt = 0;
    cnt++;
    static int my_counter = 0;
    if (cnt == 200) {
        qDebug() << "stopped ~~~~~~~~~~~~~~~~~~~~~~~~~";
        if (TEST == 0) {
            base = false;
            baseStop = true;
            initBaseAlgoritmOnce = false;
            baseInitOnce = false;
        }
        if (TEST == 1) {
            panStop = true;
            panaroma = false;
        }
    } else if (cnt == 400) {
        qDebug() << "started ~~~~~~~~~~~~~~~~~~~~~~~~~";
        if (TEST == 0) {
            initBaseAlgoritmOnce = false;
            baseStop = false;
            base = true;
            initBaseInViaBa = 1;
        }
        if (TEST == 1) {
            panaroma = true;
            panStop = false;
            panInitOnce = false;
            initAlgoritmOnce = false;
            QProcess::execute("sh -c \"rm -f /home/ubuntu/Desktop/Pan_images/*\"");
        }
    }

    if (cnt == 800) {
        cnt = 0;
        my_counter++;
        qDebug() << my_counter << "~~~~~~~~~~~~~~~~~~ü";
    }

    QTimer::singleShot(100, this, SLOT(connected()));
    if (baseStop && !base) {
        base = false;
        wrap->viaBase_Release();
        baseStop = false;
    }
    if (panStop) {
        ptzclient->setPanTiltAbs(0, 0);
        panStop = false;
        panaroma = false;
        panaromaCounter = 0;
        panInitOnce = false;
        initAlgoritmOnce = false;
        wrap->viaPan_Release();
    }
    if (panaroma) {    // Bu if i de if (panaroma && !panStop)
        if (panInit) {
            if (panaromaCounter < 4)
                ptzclient->setPanTiltPos(0,0);
            panaromaCounter++;
            if (panaromaCounter > 80) {
                panMotionStart = true;
                panInit = false;
            }
            if (panStop)
                panTiltInfo = false;
            qDebug() << "panInit " << panaromaCounter;
        } else if(panMotionStart) {
            ptzclient->setPanTiltAbs(0.0225, 0); //Hızı 90000 e denk gelecek şekilde eşitlemek için 0.225 e eşitledim, 4.000.000 * 0.0225
            panMotionStart = false;
            panTiltInfo = true;
            if (panStop)
                panTiltInfo = false;
            qDebug() << "panMotionStart";
        } else if (panTiltInfo) {
            int pan;
            int tilt;
            int zoom;
            ptzclient->getPTZPosInfo(pan, tilt, zoom);
            panVal = QString::number(pan);
            tiltVal = QString::number(tilt);
            if (panStop)
                panTiltInfo = false;
            //qDebug() << "panTiltInfo";
        } else if (panStop && !panTiltInfo && !panMotionStart && !panInit) {
//            qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~Stopping Panaroma";
//            ptzclient->setPanTiltAbs(0, 0);
//            panaroma = false;
//            panaromaCounter = 0;
//            panInitOnce = false;
//            initAlgoritmOnce = false;
//            wrap->viaPan_Release();
//            qDebug() << "panaroma counter is " << panaromaCounter;
        }
    }
}

QString SmartStreamer::doPtzCommand(QString ascii)
{
    QByteArray ba = ascii.toLatin1();
    int len = ba.length();
    char *el = ba.data();
    char checksum = el[0];
    for (int i = 1; i < len ; i++) {
        checksum = checksum ^ el[i];
    }
    QString data = QString("%1%2>").arg(ascii).arg(QString::number((uint)checksum, 16).toUpper());
    return data;
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
	rtspServer->setNetworkInterface("eth0");
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
    screenBuffer = (uchar *)buf.constData();
    width = buf.constPars()->videoWidth;
    height = buf.constPars()->videoHeight;
#if 0
    if (cuConf->getActiveMode() == CudaConfigurations::CudaList::CU_PAN_MODE) {
        if (wrap->pan_enabled == false) {
            ptzclient->setPanTiltPos(0,0);
            wrap->pan_enabled = true;
        }
        wrap->viaPan(buf);

        if (PRINT_BUFS)
            ffDebug() << "Current mode is Panorama" << wrap->meta[0] << wrap->meta[1];
    }
    else if (cuConf->getActiveMode() == CudaConfigurations::CudaList::CU_MOT_MODE) {
        wrap->viaBase(buf);
        if (PRINT_BUFS)
            ffDebug() << "Current mode is Motion Detection";
    }
#endif
#ifdef HAVE_VIA_WRAPPER
    //qDebug() << "Panorama ~~~~~~~~~~~~~~~~~~~~~~ " << panaroma;
    if (panaroma) {
        if (panaromaCounter > 79) {
            if (initAlgoritmOnce) {
                initPanInViaBa = 0;
            }
            float pan_tilt_zoom_read[3];
            pan_tilt_zoom_read[0] = QString(panVal).toFloat()/17777.777;
            pan_tilt_zoom_read[1] = QString(tiltVal).toFloat()/17777.777;
            pan_tilt_zoom_read[2] = QString(speed).toFloat();
            wrap->viaPan(buf,pan_tilt_zoom_read,initPanInViaBa);
            if (wrap->meta[0] != 0)
                panStop = true;
            else panStop = false;
            initAlgoritmOnce = true;
        }

    } else if (base) {
        if (initBaseAlgoritmOnce) {
            initBaseInViaBa = 0;
        }
        wrap->viaBase(buf,initBaseInViaBa);
        initBaseAlgoritmOnce = true;
        if (sei) {
            QByteArray ba = QByteArray((char *)wrap->meta, 4096);
            sei->processMessage(ba);
        }
    }
#endif
	return 0;
}

int SmartStreamer::processMainRGB(const RawBuffer &buf)
{
	if (PRINT_BUFS)
		ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
			  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
	return 0;
}

int SmartStreamer::processScaledRGB(const RawBuffer &buf)
{
	if (PRINT_BUFS)
		ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
			  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
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

grpc::Status SmartStreamer::SetCurrentMode(grpc::ServerContext *context, const OrionCommunication::SetModeQ *request, OrionCommunication::AppCommandResult *response)
{
    Q_UNUSED(context)
    Q_UNUSED(response)
    int value = request->mode();
    qDebug() << value << request;
    if (CudaConfigurations::CudaList::CU_NONE == value) {
        cuConf->changeMode(CudaConfigurations::CudaList::CU_NONE);
        base = false;
        panaroma = false;
    }
    else if (CudaConfigurations::CudaList::CU_PAN_MODE == value) {
        cuConf->changeMode(CudaConfigurations::CudaList::CU_PAN_MODE);
        panaroma = true;
        QProcess::execute("sh -c \"rm -f /home/ubuntu/Desktop/Pan_images/*\"");
    }
    else if (CudaConfigurations::CudaList::CU_MOT_MODE == value) {
        cuConf->changeMode(CudaConfigurations::CudaList::CU_MOT_MODE);
        base = true;
    }
    else if (CudaConfigurations::CudaList::CU_REC_MODE == value)
        cuConf->changeMode(CudaConfigurations::CudaList::CU_REC_MODE);
    return grpc::Status::OK;
}

grpc::Status SmartStreamer::GetCurrentMode(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::SetModeQ *response)
{
    Q_UNUSED(context)
    Q_UNUSED(request)
    if (panaroma && !base)
        response->set_mode(response->PANAROMA);
    else if (base && !panaroma)
        response->set_mode(response->MOTION_DETECTION);
    else if (!base && !panaroma)
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
    bool lastFrame = false;
    bool allFrames = false;
    if (request->modeframe() == request->LastFrame)
        lastFrame = true;
    else if (request->modeframe() == request->AllFrames)
        allFrames = true;
    QString next = "/home/ubuntu/Desktop/Pan_images/Pan%1.jpg";
    QString nextFinal = "/home/ubuntu/Desktop/Pan_images/PanFinal.jpg";
    int progress = 0;
    int nameLastValue = 200;
    while (1) {
        qDebug() << "-------------------------------------------------------" << panaroma << panStop;
        if (!panaroma) {
            return Status::CANCELLED;
        }
        QString picture;
        picture = next.arg(QString::number(nameLastValue), 4, QChar('0'));
        qDebug() << "-------------------------------------------------------" << picture << QFile::exists(picture);    
        if (lastFrame) {
            picture = nextFinal;
            if (!QFile::exists(picture)) { // check last frame enabled.
                sleep(1);
                continue;
            }
            progress = 9;
        }
        if (QFile::exists(nextFinal)) { // check each frames and running standart progress
            picture = nextFinal;
            progress = 9;
        } else if (!QFile::exists(picture)) {
            sleep(1);
            continue;
        }
        sleep(1);
        QImage im(picture);
        OrionCommunication::PanoramaFrame panFrames;
        panFrames.set_valid(true);
        progress = progress + 1;
        panFrames.set_progress(progress * 10);
        QByteArray ba;
        QBuffer qbuf(&ba);
        qbuf.open(QIODevice::WriteOnly);
        im.save(&qbuf, "JPG");
        panFrames.set_framedata(ba.data(), ba.size());
        writer->Write(panFrames);
        nameLastValue = nameLastValue + 200;
        if (picture == nextFinal)
            break;
    }
    return Status::OK;
}

grpc::Status SmartStreamer::RunPanaroma(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
    Q_UNUSED(context)
    Q_UNUSED(request)
    Q_UNUSED(response)
    if (base)
        base = false;
    panaroma = true;
    panStop = false;
    panInitOnce = false;
    initAlgoritmOnce = false;
    QProcess::execute("sh -c \"rm -f /home/ubuntu/Desktop/Pan_images/*\"");
    return Status::OK;
}

grpc::Status SmartStreamer::RunMotion(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
    Q_UNUSED(context)
    Q_UNUSED(request)
    Q_UNUSED(response)
    if (panaroma)
        panaroma = false;    
    base = true;
    baseStop = false;
    initBaseAlgoritmOnce = false;
    initBaseInViaBa = 1;
    return Status::OK;
}

grpc::Status SmartStreamer::StopPanaroma(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
    Q_UNUSED(context)
    Q_UNUSED(request)
    Q_UNUSED(response)
    panStop = true;
    panaroma = false;
    return Status::OK;
}

grpc::Status SmartStreamer::StopMotion(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
    base = false;
    baseStop = true;
    initBaseAlgoritmOnce = false;
    baseInitOnce = false;
    return Status::OK;
}

grpc::Status SmartStreamer::GetScreenShot(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::ScreenFrame *response)
{
    Q_UNUSED(context)
    Q_UNUSED(request)
    response->set_width(width);
    response->set_height(height);
    QByteArray ba(reinterpret_cast<const char*>(screenBuffer));
    response->set_frame(ba.data(), ba.size());
    return Status::OK;
}

grpc::Status SmartStreamer::SetMotionDetectionParameters(grpc::ServerContext *context, const OrionCommunication::TRoi *request, OrionCommunication::AppCommandResult *response)
{
    Q_UNUSED(context)
    Q_UNUSED(response)
    qDebug() << "polygon byte size " << request->polygon().ByteSize();
    if (request->polygon().ByteSize() < 20) {
        response->set_err(1);
        return Status::CANCELLED;
    }

    QFile f("points.txt");
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        response->set_err(2);
        return Status::CANCELLED;
    }

    QTextStream out(&f);
    qDebug() << "Polygon size" << request->polygon().points_size();
    for (int i = 0; i < request->polygon().points_size(); i++) {
        qDebug() << "Polygon.x : " << request->polygon().points(i).x();
        qDebug() << "Polygon.y : " << request->polygon().points(i).y();
        out << request->polygon().points(i).x() << "\n";
        out << request->polygon().points(i).y() << "\n";
    }
    for (int i = 0; i < request->polygon().points_size(); i++) {
        out << 0 << "\n";
        out << 0 << "\n";
    }
    // rect1
    out << request->rect1().upperleft().x() << "\n";
    out << request->rect1().upperleft().y() << "\n";
    out << request->rect1().bottomright().y() << "\n";
    out << request->rect1().bottomright().y() << "\n";
    // rect2
    out << request->rect2().upperleft().x() << "\n";
    out << request->rect2().upperleft().y() << "\n";
    out << request->rect2().bottomright().y() << "\n";
    out << request->rect2().bottomright().y() << "\n";
    qDebug() << "Rectangle1.x : " << request->rect1().upperleft().x();
    qDebug() << "Rectangle1.y : " << request->rect1().upperleft().y();
    qDebug() << "Rectangle1.x : " << request->rect1().bottomright().y();
    qDebug() << "Rectangle1.y : " << request->rect1().bottomright().y();
    qDebug() << "Rectangle1.x : " << request->rect2().upperleft().x();
    qDebug() << "Rectangle1.y : " << request->rect2().upperleft().y();
    qDebug() << "Rectangle1.x : " << request->rect2().bottomright().y();
    qDebug() << "Rectangle1.y : " << request->rect2().bottomright().y();
    f.close();
    return Status::OK;
}

void SmartStreamer::timeout()
{
#if 0
    static int cnt = 0;
    cnt++;
    if (cnt == 5)
        cuConf->changeMode(CudaConfigurations::CudaList::CU_PAN_MODE);
    else if (cnt == 30)
        cuConf->changeMode(CudaConfigurations::CudaList::CU_MOT_MODE);
	qDebug() << rtp->getOutputQueue(0)->getFps()
				<< dec->getOutputQueue(0)->getFps()
				<< vout->getWidget()->getDropCount();
#endif
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


