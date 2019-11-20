#include "tx1streamer.h"
#include "seiinserter.h"
#include "mjpegserver.h"
#include "mjpegserver.h"
#include "kardelenapi.h"
#include "applicationinfo.h"
#include "simpleapiserver.h"
#include "internalrecorder.h"
#include "alarmgeneratorelement.h"
#include "algorithm/algorithmgrpcserver.h"
#include "algorithm/basealgorithmelement.h"
#include <ecl/ptzp/ptzpdriver.h>

#include <lmm/debug.h>
#include <lmm/v4l2input.h>
#include <lmm/fileoutput.h>
#include <lmm/textoverlay.h>
#include <lmm/bufferqueue.h>
#include <lmm/videoscaler.h>
#include <lmm/qtvideooutput.h>
#include <lmm/tools/unittimestat.h>
#include <lmm/rtp/rtptransmitter.h>
#include <lmm/tx1/tx1jpegencoder.h>
#include <lmm/tx1/tx1videoencoder.h>
#include <lmm/pipeline/functionpipeelement.h>
#include <lmm/rtsp/basertspserver.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/ffmpeg/ffmpegcolorspace.h>

extern "C" {
#include <libavformat/avformat.h>
}

#include "streamercommon.h"

#include <QUuid>
#include <QPainter>

struct fg {
	int mode;
	QImage img;
	int cnt;
};

static fg x;

TX1Streamer::TX1Streamer(QObject *parent)
	: BaseStreamer(parent)
{
	grpcserv = AlgorithmGrpcServer::instance();
	x.mode = 0;
	algos = GPU_FREE;
	algosPending = NONE;
	algen = new alarmGeneratorElement;
	AlgorithmGrpcServer::instance()->setAlgorithmManagementInterface(this);
	secondStream = true;
	thirdStream = true;
	fourthStream = true;
	enablePreview = false;
	motionExtraEnabled = false;
	videoRecordingEnabled = false;
	motionAlarmSource = QSharedPointer<MotionAlarmSource>(new MotionAlarmSource);
	trackAlarmSource = QSharedPointer<TrackAlarmSource>(new TrackAlarmSource);
	tamperAlarmSource = QSharedPointer<GenericAlarmSource>(new GenericAlarmSource("tamper_detection"));
	AlgorithmGrpcServer::instance()->addAlarmSource(motionAlarmSource);
	AlgorithmGrpcServer::instance()->addAlarmSource(trackAlarmSource);
	AlgorithmGrpcServer::instance()->addAlarmSource(tamperAlarmSource);
}

int TX1Streamer::start()
{
	QSize res0;
	BaseLmmPipeline *p1 = createYUV420Pipeline(res0);
	if (p1) {
		finishGeneric420Pipeline(p1, res0);
		connect(SimpleApiServer::instance(), SIGNAL(urlRequested(QUrl)), this, SLOT(apiUrlRequested(QUrl)));
		
		if (motion->isAutoStart() && privacy->isAutoStart()) {
			motionExtraEnabled = true;
			runAlgorithm(0);
		} else if (motion->isAutoStart()) {
			runAlgorithm(0);
		} else if (privacy->isAutoStart()) {
			runAlgorithm(2);
		} else if (track->isAutoStart()) {
			runAlgorithm(1);
		} else if (panchange->isAutoStart()) {
			runAlgorithm(3);
		} else if (face->isAutoStart()) {
			runAlgorithm(4);
		}
	}

	return BaseStreamer::start();
}

/**
 * @brief TX1Streamer::runAlgorithm
 * @param channel
 * @return
 *
 * Algorithm state management:
 *
 * We have some requirements on current algorithm state managemenet:
 *	1. If both bypass and motion are selected, we need to enable motion with bypas features selected.
 *	2. Track can not be openen with other algorithms.
 *	3. We shouldn't init and use any algorithm, without completely releasing previous running one.
 *
 * We need to serialize these start/stop operations with algorithm state transitions. So we do not
 * perform actual state transitions in run/stop functions (expect for very trivial cases)
 *
 * Real state transitions occur in checkAlgoState()
 */
int TX1Streamer::runAlgorithm(int channel)
{
	BaseAlgorithmElement *el = ApplicationInfo::instance()->getAlgorithmInstance(channel);
	if (el->getState() != BaseAlgorithmElement::UNKNOWN)
		return -EBUSY;

	ffDebug() << channel << el << motion << track << privacy;
	if (el == track) {
		/* checking for Requirement 2 */
		if (motion->getState() != BaseAlgorithmElement::UNKNOWN)
			return -EINVAL;
		if (privacy->getState() != BaseAlgorithmElement::UNKNOWN)
			return -EINVAL;
		if (panchange->getState() != BaseAlgorithmElement::UNKNOWN)
			return -EINVAL;
		/* we have nothing running so track can start */
		algosPending = TRACK_RUNNING;
	} else if (el == motion) {
		/* Checking for Requirement 2 */
		if (track->getState() != BaseAlgorithmElement::UNKNOWN)
			return -EINVAL;
		if (panchange->getState() != BaseAlgorithmElement::UNKNOWN)
			return -EINVAL;
		if (privacy->getState() != BaseAlgorithmElement::UNKNOWN) {
			/* Requirement 1 */
			privacy->setState(BaseAlgorithmElement::STOPALGO);
			algosPending = MOTION_RUNNING;
			motionExtraEnabled = true;
			return 0;
		}
		/* we have nothing running so motion can start */
		algosPending = MOTION_RUNNING;
	} else if (el == privacy) {
		/* Checking for Requirement 2 */
		if (track->getState() != BaseAlgorithmElement::UNKNOWN)
			return -EINVAL;
		if (panchange->getState() != BaseAlgorithmElement::UNKNOWN)
			return -EINVAL;
		if (motion->getState() != BaseAlgorithmElement::UNKNOWN) {
			StabilizationAlgorithmElement *sel = (StabilizationAlgorithmElement *)privacy;
			((MotionAlgorithmElement *)motion)->enableExtra(sel->getPrivacy(), sel->getStabilization());
			/* Requirement 1 */
			motionExtraEnabled = true;
			return 0;
		}
		/* we have nothing running so bypass can start */
		algosPending = PRIVACY_RUNNING;
	} else if (el == panchange) {
		/* checking for Requirement 2 */
		if (motion->getState() != BaseAlgorithmElement::UNKNOWN)
			return -EINVAL;
		if (privacy->getState() != BaseAlgorithmElement::UNKNOWN)
			return -EINVAL;
		if (track->getState() != BaseAlgorithmElement::UNKNOWN)
			return -EINVAL;
		/* we have nothing running so track can start */
		algosPending = DIFF_RUNNING;
	} else if (el == face) {
		algosPending = FACE_RUNNING;
		/* face recides in RGB portion */
		enableRGBPortion(true);
	}

	return 0;
}

int TX1Streamer::stopAlgorithm(int channel)
{
	BaseAlgorithmElement *el = ApplicationInfo::instance()->getAlgorithmInstance(channel);
	if (el->getState() != BaseAlgorithmElement::PROCESS) {
		if (el == privacy) {
			/* we may have privacy running thru motion */
			if (!motionExtraEnabled)
				return -EINVAL;
			StabilizationAlgorithmElement *sel = (StabilizationAlgorithmElement *)privacy;
			((MotionAlgorithmElement *)motion)->enableExtra(sel->getPrivacy(), sel->getStabilization());
			motionExtraEnabled = false;
			return 0;
		}
		return -EBUSY;
	}
	if (el == face)
		enableRGBPortion(false);
	el->setState(BaseAlgorithmElement::STOPALGO);
	if (el == motion && motionExtraEnabled) {
		/* we need to enable privacy masking */
		algosPending = PRIVACY_RUNNING;
	}
	return 0;
}

TX1Streamer::~TX1Streamer()
{

}

void TX1Streamer::apiUrlRequested(const QUrl &url)
{
	QString fname = url.toString();
	QStringList actions = fname.split("/", QString::SkipEmptyParts);
	ffDebug() << actions;
	if (actions[0] != "control")
		return;
	if (actions.size() < 2)
		return;
	if (actions[1] == "algorithm") {
		if (actions.size() < 4)
			return;
		int index = actions[2].toInt();
		QString action = actions[3];
		BaseAlgorithmElement *el = nullptr;
		if (index == 0) {
			el = motion;
		} else if (index == 2) {
			el = privacy;
		} else if (index == 1) {
			el = track;
		} else if (index == 4) {
			el = face;
		} else
			return;
		if (action == "jsonreload")
			el->reloadJson();
		else if (action == "start")
			runAlgorithm(index);
		else if (action == "pause")
			el->setPassThru(true);
		else if (action == "resume")
			el->setPassThru(false);
		else if (action == "stop")
			stopAlgorithm(index);
		else if (action == "restart")
			el->restart();
	} else if (actions[1] == "reload") {
		if (actions.size() < 3)
			return;
		QString node = actions[2];
		if (node == "overlay")
			StreamerCommon::reloadJson(textOverlay);
		if (node == "encoders") {
			/* encoders don't support hot-reload */
			QTimer::singleShot(1000, QCoreApplication::instance(), SLOT(quit()));
		}
	} else if (actions[1] == "fg") {
		x.mode = actions[2].toInt();
	}
}

int TX1Streamer::checkSeiAlarm(const RawBuffer &buf)
{
	checkAlgoState();
	if (sei && buf.constPars()->metaData.size()) {
		QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);
		QByteArray seiData = hash["motion_results"].toByteArray();
		sei->processMessage(seiData);
		if (seiData.size() == 0)
			sei->clearLastSEIMessage();

		if (KardelenAPIServer::instance()) {
			/* notify kardelen as well */
			if (motion->getState() == BaseAlgorithmElement::PROCESS) {
				algen->generateAlarmStructure((uchar *)seiData.constData(), alarmGeneratorElement::MOTION);
				alarmGeneratorElement::AlarmInfo *info = algen->getAlarmInfo();
				if (info->target.size())
					KardelenAPIServer::instance()->setMotionObjects(info->target);
			} else if (track->getState() == BaseAlgorithmElement::PROCESS) {
				QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);
				QByteArray seiData = hash["track_results"].toByteArray();
				algen->generateAlarmStructure((uchar *)seiData.constData(), alarmGeneratorElement::TRACKING);
				alarmGeneratorElement::AlarmInfo *info = algen->getAlarmInfo();
				if (info->target.size())
					KardelenAPIServer::instance()->setMotionObjects(info->target);
			}
		}

	} else if (buf.constPars()->metaData.size() == 0) {
		sei->clearLastSEIMessage();
	}
	return 0;
}

int TX1Streamer::frameGenerator(const RawBuffer &buf)
{
	if (x.mode == 0)
		return 0;
	if (x.img.isNull()) {
		x.img.load(":/oa.png");
		x.cnt = 0;
		x.mode = 2;
	}
	if (x.mode == 1) {
		if (!x.cnt++) {
			//memset((void *)buf.constData(), 0x80, buf.size());
			QImage im((uchar *)buf.constData(), buf.constPars()->videoWidth, buf.constPars()->videoHeight, QImage::Format_Grayscale8);
			//im.fill(0);
			QPainter p(&im);
			p.setPen(Qt::white);
			p.setBrush(Qt::white);
			p.setFont(QFont("Arial", 48));
			p.drawImage(0, 0, x.img);

			QString text = "funny";
			//p.drawText(im.rect(), Qt::AlignVCenter | Qt::AlignHCenter, text);
		} else if (x.cnt == 25)
			x.cnt = 0;
	} else if (x.mode == 2) {
		memset((void *)buf.constData(), 0x80, buf.size());
		QImage im((uchar *)buf.constData(), buf.constPars()->videoWidth, buf.constPars()->videoHeight, QImage::Format_Grayscale8);
		im.fill(0);
		QPainter p(&im);
		p.setPen(Qt::white);
		p.setBrush(Qt::white);
		p.setFont(QFont("Arial", 48));
		p.drawImage(0, 0, x.img);
		QString text = "funny";
		p.drawText(im.rect(), Qt::AlignVCenter | Qt::AlignRight, text);
	}

	return 0;
}

int TX1Streamer::notifyGrpcForAlarm(const RawBuffer &buf)
{
	if (motion->getState() == BaseAlgorithmElement::PROCESS) {
		notifyGrpcForMotionAlarm(buf);
	}

	if (track->getState() == BaseAlgorithmElement::PROCESS) {
		QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);
		QByteArray seiData = hash["track_results"].toByteArray();
		algen->generateAlarmStructure((uchar *)seiData.constData(), alarmGeneratorElement::TRACKING);
		alarmGeneratorElement::AlarmInfo *info = algen->getAlarmInfo();
		if (info->target.size()) {
			QJsonArray trackObjects;
			for (uint i = 0; i < info->target.size(); i++) {
				alarmGeneratorElement::TargetStruct ts = info->target[i];
				QJsonObject obj;
				obj["x"] = ts.topLeftX;
				obj["y"] = ts.topLeftY;
				obj["w"] = ts.widthOfTarget;
				obj["h"] = ts.heightOfTarget;
				trackObjects.append(obj);
			}
			QJsonObject obj;
			obj["objects"] = trackObjects;
			const QByteArray jsondata = QJsonDocument(obj).toJson();
			const RawBuffer &jpegbuf = jpegQueue->getLast();
			QByteArray ba = QByteArray::fromRawData((const char *)jpegbuf.constData(), jpegbuf.size());
			trackAlarmSource->produce(((TrackAlgorithmElement *)(track))->getTrackID(),
									  QString::fromUtf8(jsondata), ba);
		}
	}

	return 0;
}

int TX1Streamer::notifyGrpcForMotionAlarm(const RawBuffer &buf)
{
	/* convert to json */
	QHash<QString, QVariant> hash = RawBuffer::deserializeMetadata(buf.constPars()->metaData);
	QByteArray seiData = hash["motion_results"].toByteArray();
	algen->generateAlarmStructure((uchar *)seiData.constData(), alarmGeneratorElement::MOTION);
	alarmGeneratorElement::AlarmInfo *info = algen->getAlarmInfo();
	QJsonArray motionObjects;
	for (uint i = 0; i < info->target.size(); i++) {
		alarmGeneratorElement::TargetStruct ts = info->target[i];
		QJsonObject obj;
		obj["x"] = ts.topLeftX;
		obj["y"] = ts.topLeftY;
		obj["w"] = ts.widthOfTarget;
		obj["h"] = ts.heightOfTarget;
		motionObjects.append(obj);
	}

	QJsonObject obj;
	obj["objects"] = motionObjects;
	const QByteArray jsondata = QJsonDocument(obj).toJson();
	const RawBuffer &jpegbuf = jpegQueue->getLast();
	QByteArray ba = QByteArray::fromRawData((const char *)jpegbuf.constData(), jpegbuf.size());
	if (motionObjects.size())
		motionAlarmSource->produce("", QString::fromUtf8(jsondata), ba);
	else
		motionAlarmSource->notifyNoMotion("", "", ba);

	if (info->tamper)
		tamperAlarmSource->produce("severity", "1.0");

	return 0;
}

int TX1Streamer::processBuffer(const RawBuffer &buf)
{
	Q_UNUSED(buf);
	return 0;
}

int TX1Streamer::recordIfNvrDead(const RawBuffer &buf)
{
#if 0
	if (!recorder->isNvrDead())
		return 0;
	recorder->record(static_cast<char const*>(buf.constData()), buf.size());
#endif
	return 0;
}

void TX1Streamer::enableRGBPortion(bool en)
{
	yuv2rgb->setPassThru(!en);
}

void TX1Streamer::checkAlgoState()
{
	//ffDebug() << algos;
	switch (algos) {
	case TO_GPU_FREE:
		if (motion->getState() != BaseAlgorithmElement::UNKNOWN)
			break;
		if (privacy->getState() != BaseAlgorithmElement::UNKNOWN)
			break;
		if (panchange->getState() != BaseAlgorithmElement::UNKNOWN)
			break;
		if (track->getState() != BaseAlgorithmElement::UNKNOWN)
			break;
		algos = GPU_FREE;
		break;
	case GPU_FREE:
		if (algosPending != NONE) {
			if (algosPending == MOTION_RUNNING) {
				StabilizationAlgorithmElement *sel = (StabilizationAlgorithmElement *)privacy;
				((MotionAlgorithmElement *)motion)->enableExtra(sel->getPrivacy(), sel->getStabilization());
				algos = TO_MOTION_RUNNING;
				motion->setState(BaseAlgorithmElement::INIT);
			}
			if (algosPending == PRIVACY_RUNNING) {
				algos = TO_PRIVACY_RUNNING;
				privacy->setState(BaseAlgorithmElement::INIT);
			}
			if (algosPending == TRACK_RUNNING) {
				algos = TO_TRACK_RUNNING;
				track->setState(BaseAlgorithmElement::INIT);
			}
			if (algosPending == DIFF_RUNNING) {
				algos = TO_DIFF_RUNNING;
				panchange->setState(BaseAlgorithmElement::INIT);
			}
			if (algosPending == FACE_RUNNING) {
				algos = TO_FACE_RUNNING;
				face->setState(BaseAlgorithmElement::INIT);
			}
			algosPending = NONE;
		}
		break;
	case TO_PRIVACY_RUNNING:
		if (privacy->getState() != BaseAlgorithmElement::PROCESS)
			break;
		algos = PRIVACY_RUNNING;
		break;
	case PRIVACY_RUNNING:
		if (privacy->getState() != BaseAlgorithmElement::PROCESS)
			algos = TO_GPU_FREE;
		break;
	case TO_MOTION_RUNNING:
		if (motion->getState() != BaseAlgorithmElement::PROCESS)
			break;
		algos = MOTION_RUNNING;
		break;
	case MOTION_RUNNING:
		if (motion->getState() != BaseAlgorithmElement::PROCESS)
			algos = TO_GPU_FREE;
		break;
	case TO_TRACK_RUNNING:
		if (track->getState() != BaseAlgorithmElement::PROCESS)
			break;
		algos = TRACK_RUNNING;
		break;
	case TRACK_RUNNING:
		if (track->getState() != BaseAlgorithmElement::PROCESS)
			algos = TO_GPU_FREE;
		break;
	case TO_DIFF_RUNNING:
		if (panchange->getState() != BaseAlgorithmElement::PROCESS)
			break;
		algos = DIFF_RUNNING;
	case DIFF_RUNNING:
		if (panchange->getState() != BaseAlgorithmElement::PROCESS)
			algos = TO_GPU_FREE;
		break;
	case TO_FACE_RUNNING:
		if (face->getState() != BaseAlgorithmElement::PROCESS)
			break;
		algos = FACE_RUNNING;
		break;
	case FACE_RUNNING:
		if (face->getState() != BaseAlgorithmElement::PROCESS)
			algos = TO_GPU_FREE;
		break;
	case NONE:
		break;
	}

	int overlayCnt = ((TextOverlay *)textOverlay)->getFieldCount();

	QStringList zrList;
	if(overlayCnt == 4) {
		PtzpDriver *driver = ApplicationInfo::instance()->getPtzpDriver(0);
		int headCnt = driver->getHeadCount();
		for (int i= 0; i < headCnt; ++i) {
			PtzpHead *headpt = driver->getHead(i);
			if(headpt->getCapabilities() & PtzpHead::CAP_ZOOM) {
				zrList.append(QString("X%1").arg(headpt->getZoomRatio()));
			}
		}
		((TextOverlay *)textOverlay)->setOverlayFieldText(1, zrList.join(" "));
	}
#if 0
	((TextOverlay *)textOverlay)->setOverlayFieldText(3, QString("%1,%2").arg(algos).arg(algosPending));
	((TextOverlay *)textOverlay)->setOverlayFieldText(4, QString("m=%1 t=%2 p=%3 diff=%4 p=%5")
													  .arg(motion->getState())
													  .arg(track->getState())
													  .arg(privacy->getState())
													  .arg(panchange->getState())
													  .arg(motionExtraEnabled)
													  );
#endif
}

void TX1Streamer::finishGeneric420Pipeline(BaseLmmPipeline *p1, const QSize &res0)
{
	BufferQueue* queue = new BufferQueue;
	jpegQueue = new BufferQueue;
	jpegQueue->setQueueSize(2);

	motion = ApplicationInfo::instance()->createAlgorithm("motion");
	track = ApplicationInfo::instance()->createAlgorithm("track");
	privacy = ApplicationInfo::instance()->createAlgorithm("privacy");
	panchange = ApplicationInfo::instance()->createAlgorithm("panchange");
	face = ApplicationInfo::instance()->createAlgorithm("faceDetection");

	enc0 = StreamerCommon::createEncoder(0);
	((TX1VideoEncoder *)enc0)->setOutputResolution(res0.width(), res0.height());
	enc1 = StreamerCommon::createEncoder(1);
	enc2 = StreamerCommon::createEncoder(2);
	enc3 = StreamerCommon::createEncoder(3);

	VideoScaler* downScalar1 = new VideoScaler;
	VideoScaler* downScalar2 = new VideoScaler;
	VideoScaler* downScalar3 = new VideoScaler;
	VideoScaler* downScalarJpeg = new VideoScaler;
	QSize res1 = ((TX1VideoEncoder *)enc1)->getOutputResolution();
	QSize res2 = ((TX1VideoEncoder *)enc2)->getOutputResolution();
	QSize res3 = ((TX1VideoEncoder *)enc3)->getOutputResolution();
	float fps0 = ((TX1VideoEncoder *)enc0)->getFps();
	float fps1 = ((TX1VideoEncoder *)enc1)->getFps();
	float fps2 = ((TX1VideoEncoder *)enc2)->getFps();
	float fps3 = ((TX1VideoEncoder *)enc3)->getFps();
	downScalar1->setOutputResolution(res1.width(), res1.height());
	downScalar2->setOutputResolution(res2.width(), res2.height());
	downScalar3->setOutputResolution(res3.width(), res3.height());
	downScalarJpeg->setOutputResolution(960, 540);

	TX1JpegEncoder *jenc = new TX1JpegEncoder;
	MjpegElement *jpegel = new MjpegElement(13789);
	textOverlay = StreamerCommon::createOverlay();

	sei = new SeiInserter;
	sei->setAlarmTemplate("sei_alarm_template.xml");

	RtpTransmitter *rtpout = StreamerCommon::createRtpTransmitter(fps0, 0);
	RtpTransmitter *rtpout2 = StreamerCommon::createRtpTransmitter(fps1, 1);
	RtpTransmitter *rtpout3 = StreamerCommon::createRtpTransmitter(fps2, 2);
	RtpTransmitter *rtpout4 = StreamerCommon::createRtpTransmitter(fps3, 3);

	/* rgb portion is passthru by default */
	yuv2rgb = new  VideoScaler;
	yuv2rgb->setOutputFormat(AV_PIX_FMT_ARGB);
	yuv2rgb->setMode(1);
	yuv2rgb->setPassThru(true);
	rgb2yuv = new  VideoScaler;
	rgb2yuv->setOutputFormat(AV_PIX_FMT_YUV420P);
	rgb2yuv->setMode(1);

	p1->setQuitOnThreadError(true);
	p1->append(privacy);
	p1->append(motion);
	p1->append(track);
	p1->append(panchange);

	/* rgb portion */
	p1->append(yuv2rgb);
	p1->append(face);
	p1->append(rgb2yuv);

	p1->append(newFunctionPipe(TX1Streamer, this, TX1Streamer::checkSeiAlarm));
	p1->append(newFunctionPipe(TX1Streamer, this, TX1Streamer::notifyGrpcForAlarm));
	p1->append(textOverlay);
	p1->append(queue);
	p1->append(enc0);
	p1->append(sei);
	if (videoRecordingEnabled)
		p1->append(newFunctionPipe(TX1Streamer, this, TX1Streamer::recordIfNvrDead));
	p1->append(rtpout);
	p1->end();

	if (secondStream) {
		BaseLmmPipeline *p2 = addPipeline();
		p2->append(queue);
		if (res0 != res1)
			p2->append(downScalar1);
		p2->append(enc1);
		p2->append(rtpout2);
		p2->end();
	}

	BaseLmmPipeline *p3 = addPipeline();
	p3->append(queue);
	p3->append(downScalarJpeg);
	p3->append(newFunctionPipe(TX1Streamer, this, TX1Streamer::frameGenerator));
	p3->append(jenc);
	p3->append(jpegel);
	p3->append(jpegQueue);
	p3->end();

	if (thirdStream) {
		BaseLmmPipeline *p4 = addPipeline();
		p4->append(queue);
		if (res0 != res2)
			p4->append(downScalar2);
		p4->append(enc2);
		p4->append(rtpout3);
		p4->end();
	}

	if (fourthStream) {
		BaseLmmPipeline *p5 = addPipeline();
		p5->append(queue);
		if (res0 != res3)
			p5->append(downScalar3);
		p5->append(enc3);
		p5->append(rtpout4);
		p5->end();
	}
	if (enablePreview) {
		BaseLmmPipeline *p7 = addPipeline();
		p7->append(queue);
		VideoScaler *sc0 = new VideoScaler;
		sc0->setOutputResolution(640, 360);
		VideoScaler* rgbConv1 = new VideoScaler;
		rgbConv1->setOutputFormat(AV_PIX_FMT_ARGB);
		rgbConv1->setMode(1);
		p7->append(sc0);
		p7->append(rgbConv1);
		QtVideoOutput *vout = new QtVideoOutput;
		vout->getWidget()->setGeometry(0, 0, 640, 360);
		p7->append(vout);
		p7->end();
	}

	QList<RtpTransmitter *> rtplist;
	rtplist << rtpout;
	rtplist << rtpout2;
	rtplist << rtpout3;
	rtplist << rtpout4;
	addExtraRtpTransmitters(rtplist);
	StreamerCommon::createRtspServer(rtplist);

	queue->getOutputQueue(0)->setRateReduction(25, fps0);
	if (secondStream)
		queue->getOutputQueue(1)->setRateReduction(25, fps1);
	//queue->getOutputQueue(2)->setRateReduction(25, fps0);
	if (thirdStream)
		queue->getOutputQueue(3)->setRateReduction(25, fps2);
	if (fourthStream)
		queue->getOutputQueue(4)->setRateReduction(25, fps3);

#if 0
	if (videoRecordingEnabled) {
		recorder = new InternalRecorder;
		recorder->start();
	}
#endif

	/* on TX1 we have IPv4 fragmentation on by default, close it. */
	QFile f("/proc/sys/net/ipv4/ip_no_pmtu_disc");
	if (f.open(QIODevice::WriteOnly | QIODevice::Unbuffered)) {
		f.write("1\n");
		f.close();
	} else {
		mDebug("Error opening %s", qPrintable(f.fileName()));
	}
}

