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

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/channel.h>
#include <grpc++/create_channel.h>
#include <grpc++/client_context.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/credentials.h>
#include <grpc++/security/server_credentials.h>

#include <QBuffer>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::Status;
using namespace std;

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
	: BaseStreamer(parent)
{
    cuConf = new CudaConfigurations();
#ifdef HAVE_VIA_WRAPPER
	wrap = new ViaWrapper();
#endif
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
    if (cuConf->getActiveMode() == CudaConfigurations::CudaList::CU_PAN_MODE) {
        wrap->viaPan(buf);
        if (PRINT_BUFS)
            ffDebug() << "Current mode is Panorama";
    }
    else if (cuConf->getActiveMode() == CudaConfigurations::CudaList::CU_MOT_MODE) {
        wrap->viaBase(buf);
        if (PRINT_BUFS)
            ffDebug() << "Current mode is Motion Detection";
    }
#ifdef HAVE_VIA_WRAPPER
	if (wrap->alarm_flag && sei) {
		QByteArray ba = QByteArray((char *)wrap->meta, 4096);
		sei->processMessage(ba);
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
grpc::Status SmartStreamer::SetPanorama(grpc::ServerContext *context, const config::DummyInfo *request, config::AppCommandResult *response)
{
    Q_UNUSED(context)
    Q_UNUSED(request)
    Q_UNUSED(response)
    cuConf->changeMode(CudaConfigurations::CudaList::CU_PAN_MODE);
    return grpc::Status::OK;
}

grpc::Status SmartStreamer::SetMotionDetection(grpc::ServerContext *context, const config::MotionDetectionParameters *request, config::AppCommandResult *response)
{
    Q_UNUSED(context)
    Q_UNUSED(request)
    Q_UNUSED(response)
    cuConf->changeMode(CudaConfigurations::CudaList::CU_MOT_MODE);
    return grpc::Status::OK;
}

grpc::Status SmartStreamer::GetCurrentMode(grpc::ServerContext *context, const config::DummyInfo *request, config::AppCommandResult *response)
{
    Q_UNUSED(context)
    Q_UNUSED(request)
    Q_UNUSED(response)
    return grpc::Status::OK;
}

void SmartStreamer::timeout()
{
#if 0
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

