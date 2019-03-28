#include "ipstreamer.h"
#include "mjpegserver.h"
#include "seiinserter.h"
#include "algorithmmanager.h"
#include "algorithmelement.h"

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

#include <lmm/tx1/tx1videoencoder.h>
#include <lmm/v4l2output.h>
#include <lmm/v4l2input.h>
#include <lmm/x264encoder.h>
#include <lmm/ffmpeg/baselmmdemux.h>

extern "C" {
#include <libavformat/avformat.h>
}


IpStreamer::IpStreamer(QObject *parent): BaseStreamer(parent){
	qDebug() << "Info is read";
	readSourceInformation();
	qDebug() << "Info is read";
}

int IpStreamer::generatePipelineForOneSource(const QString &Url)
{
	setupAlgorithmManager();
	rtp = new RtpReceiver(this);
	rtp->useThreadedReading(true);
	rtpout = new RtpTransmitter(this);
	rtpout->forwardRtpTs(true);
	rtpout->setRtcp(false);
	rtpout->setH264SEIInsertion(true);
	rtpout->useIncomingTimestamp(true);

	/* queues will be used to de-multiplex streams */
	BufferQueue *queue = new BufferQueue;

	dec = new FFmpegDecoder;
	dec->setBufferCount(decBufferCount);
	if (pars.decWidth)
		dec->setVideoResolution(decWidth, decHeight); //352x290

	AlgorithmElement *faceAlg = new AlgorithmElement(this);
	faceAlg->setCurrentActiveAlgorithm(AlgorithmElement::FACE_DETECTION);
	algMan->registerAlgorithm(AlgorithmManager::FACE_DETECTION, faceAlg);
	faceAlg->enableAlg(true);
	faceAlg->setConfigurationElement(algMan->getAlgHandlerFor(0),AlgorithmElement::Algorithm::FACE_DETECTION);

	BaseLmmPipeline *p1 = addPipeline();
	p1->append(rtp);
	p1->append(queue);
	p1->append(dec);

	VideoScaler *downs = new VideoScaler;
	downs->setOutputResolution(1920, 1080);
	downs->setMode(0);
	p1->append(downs);

	FFmpegColorSpace *rgbConv3 = new  FFmpegColorSpace;
	rgbConv3->setOutputFormat(AV_PIX_FMT_RGB24);
	p1->append(rgbConv3);

	p1->append(faceAlg);
	p1->append(newFunctionPipe(IpStreamer, this, IpStreamer::PerformAlgorithmForYUV));

	RtpTransmitter *rtpout2 = NULL;
	if (1) {
		TX1VideoEncoder *enc2 = new TX1VideoEncoder;
		//p1->append(enc2);

		rtpout2 = new RtpTransmitter(this);
		rtpout2->forwardRtpTs(false);
		rtpout2->setRtcp(false);
		rtpout2->setH264SEIInsertion(false);
		rtpout2->useIncomingTimestamp(false);
		rtpout2->setUseAbsoluteTimestamp(false);
		rtpout2->setFrameRate(25);
		//p1->append(rtpout2);
	}
	p1->end();
	if (rtpBufferDuration) {
		rtp->getOutputQueue(0)->setBufferDuration(rtpBufferDuration);
		rtp->getOutputQueue(0)->setTimestampingMethod(ElementIOQueue::TS_DURATION);
	}
	if (decOutputInFps > 0 && decOutputOutFps > 0) {
		dec->getOutputQueue(0)->setRateReduction(decOutputInFps, decOutputOutFps);
	}

	sei = new SeiInserter;
	sei->setAlarmTemplate("sei_alarm_template.xml");
	BaseLmmPipeline *p2 = addPipeline();
	p2->append(queue);
	p2->append(sei);
	p2->append(rtpout);
	p2->end();


	rtsp = new RtspClient(this);
	rtsp->setDefaultRtpTrack(rtp);
	if (!rtspClientUser.isEmpty())
		rtsp->setAuthCredentials(rtspClientUser, rtspClientPass);//"admin", "moxamoxa"
	if (Url.startsWith("rtsp://"))
		rtsp->setServerUrl(QString("%1").arg(Url));
	else
		rtsp->setServerUrl(QString("rtsp://%1").arg(Url));
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
	rtspServer->addStream("stream1_vs", false, rtpout2);
	rtspServer->addStream("stream1_vsm",true, rtpout2, 15688);
	rtspServer->addMedia2Stream("videoTrack", "stream1_vs", false, rtpout2);
	rtspServer->addMedia2Stream("videoTrack", "stream1_vsm", true, rtpout2);

	algMan->startGrpc();

	return 0;
}

struct MultiSource
{
	class PipelineCommon {
	public:
		void setup(IpStreamer *streamer, const QString &rtspUrl, int frameRate, FunctionPipeElement *pipeel, bool softEncoder, int bitrate)
		{
			BaseLmmDemux *rtp = new BaseLmmDemux;
			rtp->setParameter("rtsp_transport", "tcp");
			rtp->setSource(rtspUrl);
			//rtp = new RtpReceiver(streamer);
			//rtp->useThreadedReading(true);
			rtpout = new RtpTransmitter(streamer);
			rtpout->forwardRtpTs(false);
			rtpout->setRtcp(false);
			rtpout->setH264SEIInsertion(false);
			rtpout->setUseAbsoluteTimestamp(true);

			dec = new FFmpegDecoder;
			dec->setH264NalChecking(false);
			dec->setBufferCount(pars.decBufferCount);
			if (pars.decWidth)
				dec->setVideoResolution(pars.decWidth, pars.decHeight); //352x290

			BaseLmmPipeline *p1 = streamer->addPipeline();
			p1->append(rtp);
			p1->append(rtpout);
			p1->append(dec);
			p1->append(pipeel);
			if (1) {
				if (softEncoder) {
					x264Encoder *senc = new x264Encoder;
					senc->setPreset("ultrafast");
					senc->setThreadCount(2);
					senc->setVideoResolution(QSize(pars.decWidth, pars.decHeight));
					senc->setBitrate(bitrate);
					p1->append(senc);
					enc = senc;
				} else {
					TX1VideoEncoder *txenc = new TX1VideoEncoder;
					txenc->setBitrate(bitrate);
					txenc->setFps(frameRate);
					enc = txenc;
					p1->append(enc);
				}

				rtpoutvs = new RtpTransmitter(streamer);
				rtpoutvs->forwardRtpTs(false);
				rtpoutvs->setRtcp(false);
				rtpoutvs->setH264SEIInsertion(false);
				rtpoutvs->useIncomingTimestamp(false);
				rtpoutvs->setUseAbsoluteTimestamp(false);
				rtpoutvs->setFrameRate(frameRate);
				p1->append(rtpoutvs);
			}
			p1->end();
			if (pars.rtpBufferDuration) {
				rtp->getOutputQueue(0)->setBufferDuration(pars.rtpBufferDuration);
				rtp->getOutputQueue(0)->setTimestampingMethod(ElementIOQueue::TS_DURATION);
			}
			if (pars.decOutputInFps > 0 && pars.decOutputOutFps > 0) {
				dec->getOutputQueue(0)->setRateReduction(pars.decOutputInFps, pars.decOutputOutFps);
			}

#if 0
			rtspClient = new RtspClient(streamer);
			rtspClient->setDefaultRtpTrack(rtp);
			if (!pars.rtspClientUser.isEmpty())
				rtspClient->setAuthCredentials(pars.rtspClientUser, pars.rtspClientPass);//"admin", "moxamoxa"
			if (rtspUrl.startsWith("rtsp://"))
				rtspClient->setServerUrl(QString("%1").arg(rtspUrl));
			else
				rtspClient->setServerUrl(QString("rtsp://%1").arg(rtspUrl));
			if (pars.enableMoxaHacks)
				rtspClient->setMoxaHacks(true);
#endif
		}

		IpStreamer::Parameters pars;
		//RtpReceiver *rtp;
		RtpTransmitter *rtpout;
		RtpTransmitter *rtpoutvs;
		//RtspClient *rtspClient;
		FFmpegDecoder *dec;
		BufferQueue *queue;
		BaseLmmElement *enc;
	};
	PipelineCommon thermal;
	PipelineCommon daytv;

};

void IpStreamer::addRtspServer(RtpTransmitter *rtpout, RtpTransmitter *rtpoutvs, int sno)
{
	rtspServer->addStream(QString("stream%1").arg(sno), false, rtpout);
	rtspServer->addStream(QString("stream%1m").arg(sno),true, rtpout, 15678);
	rtspServer->addMedia2Stream("videoTrack", QString("stream%1").arg(sno), false, rtpout);
	rtspServer->addMedia2Stream("videoTrack", QString("stream%1m").arg(sno), true, rtpout);
	if (!pars.rtspServerUser.isEmpty()) {
		rtspServer->setRtspAuthentication(BaseRtspServer::AUTH_SIMPLE);
		rtspServer->setRtspAuthenticationCredentials(pars.rtspServerUser, pars.rtspServerPass);
	}
	rtspServer->addStream(QString("stream%1_vs").arg(sno), false, rtpoutvs);
	rtspServer->addStream(QString("stream%1_vsm").arg(sno),true, rtpoutvs, 15688);
	rtspServer->addMedia2Stream("videoTrack", QString("stream%1_vs").arg(sno), false, rtpoutvs);
	rtspServer->addMedia2Stream("videoTrack", QString("stream%1_vsm").arg(sno), true, rtpoutvs);
}

int IpStreamer::generatePipelineForMultipleSource(const QStringList &Url)
{
	MultiSource *src1 = new MultiSource;
	src1->daytv.pars = pars;
	src1->daytv.setup(this, Url.at(0), 15, newFunctionPipe(IpStreamer, this, IpStreamer::PerformAlgorithmForYUV), false,
					  enc_bitrate_set.value(0));
	src1->thermal.pars = pars;
	src1->thermal.setup(this, Url.at(1), 25, newFunctionPipe(IpStreamer, this, IpStreamer::PerformAlgorithmForYUV), true,
						enc_bitrate_set.value(1));

	rtspServer = new BaseRtspServer(this, 8554);
	addRtspServer(src1->daytv.rtpout, src1->daytv.rtpoutvs, 1);
	addRtspServer(src1->thermal.rtpout, src1->thermal.rtpoutvs, 2);
	return 0;
}

int IpStreamer::PerformAlgorithmForYUV(const RawBuffer &buf)
{
	//return 0;
	//algMan->getBuffer(buf);
	//qDebug() << "Inside the perform algortihm for YUV";
	if (sei) {
		//QByteArray ba = QByteArray((char *)algMan->algHandler.meta, 4096);
		QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);
		sei->processMessage(hash["motion_results"].toByteArray());
	}
	return 0;
}

int IpStreamer::readSourceInformation()
{
	QString info;
	QFile file;
	file.setFileName("algorithm.json");
	if (file.exists()) {
		file.open(QIODevice::ReadOnly | QIODevice::Text);
	} else {
		//		mDebug("Algorithm.json file is not found");
		return -1;
	}
	info = file.readAll();
	file.close();

	QJsonDocument doc = QJsonDocument::fromJson(info.toUtf8());
	QJsonObject obj_info = doc.object();

	//Device related properties like fps,resolution are obtained with the following lines
	QJsonValue source_info = obj_info.value(QString("source"));
	QJsonObject itemized_source_info = source_info.toObject();
	QJsonObject ip_source_info = itemized_source_info["IP"].toObject();
	ipSourceEnabled = ip_source_info["enabled"].toBool();
	if (ipSourceEnabled)
		sourceAddress = ip_source_info["address"].toString();

	QJsonValue source_prop_info = obj_info.value(QString("device"));
	QJsonObject itemized_source_prop_info = source_prop_info.toObject();

	decBufferCount = itemized_source_prop_info["fps"].toInt();

	decOutputInFps = itemized_source_prop_info["in_fps"].toVariant().toFloat();
	decOutputOutFps = itemized_source_prop_info["out_fps"].toVariant().toFloat();

	QJsonObject resolution_info = itemized_source_prop_info["resolution"].toObject();
	decWidth  = resolution_info["width"].toInt();
	decHeight = resolution_info["height"].toInt();

	QJsonValue credentials = obj_info.value(QString("credentials"));
	QJsonObject credentials_info = credentials.toObject();
	rtspClientUser = credentials_info["user_name"].toString();
	rtspClientPass = credentials_info["user_passwd"].toString();

	QJsonArray encoder_info = obj_info.value(QString("encoders")).toArray();

	for (int i = 0; i < encoder_info.size(); ++i)
		enc_bitrate_set.insert(i,encoder_info.takeAt(i).toInt());
}

int IpStreamer::setupAlgorithmManager()
{
	algMan = new AlgorithmManager(this);
}

int IpStreamer::pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf)
{
	//qDebug() << buf.size() << buf.getMimeType();
	return 0;
}

int IpStreamer::process(int ch)
{
	return 0;
}

int IpStreamer::processBlocking(int ch)
{
	return 0;
}

int IpStreamer::processBuffer(const RawBuffer &buf)
{
	return 0;
}

