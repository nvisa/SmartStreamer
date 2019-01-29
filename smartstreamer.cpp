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

#include <lmm/tx1/tx1videoencoder.h>
#include <lmm/v4l2output.h>
#include <lmm/v4l2input.h>
#include <lmm/x264encoder.h>
#include <lmm/ffmpeg/baselmmdemux.h>

#include <ecl/ptzp/ptzphead.h>
#include <ecl/ptzp/aryadriver.h>
#include <ecl/ptzp/irdomedriver.h>
#include <ecl/ptzp/ptzpdriver.h>
#include <ecl/ptzp/tbgthdriver.h>
#include <ecl/drivers/patternng.h>
#include <ecl/drivers/systeminfo.h>
#include <ecl/drivers/gpiocontroller.h>
#include <ecl/net/networkaccessmanager.h>

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

class PtzNotification : public PatternNg
{
public:
	PtzNotification(SmartStreamer *s)
	{
		streamer = s;
	}

	virtual void commandUpdate(int pan, int tilt, int zoom, int c, float par1, float par2)
	{
		streamer->ptzCommandRecved(c);
		PatternNg::commandUpdate(pan, tilt, zoom, c, par1, par2);
	}

	SmartStreamer *streamer;
};

static QJsonObject loadSettings(const QString &filename)
{
	QFile f(filename);
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
		return QJsonObject();
	const QByteArray &json = f.readAll();
	f.close();
	const QJsonDocument &doc = QJsonDocument::fromJson(json);
	return doc.object();
}

static void saveSettings(const QString &filename, const QJsonObject &obj)
{
	QFile f(filename);
	if (!f.open(QIODevice::WriteOnly))
		return;
	f.write(QJsonDocument(obj).toJson());
	f.close();
}

#define GPIO_WIPER 187
#define GPIO_WATER 186

SmartStreamer::SmartStreamer(QObject *parent)
	: BaseStreamer(parent), OrionCommunication::OrionCommunicationService::Service()
{
	sets = loadSettings("settings.json");
	if (!sets.contains("encoders")) {
		/* default settings */
		QJsonArray arr;
		QJsonObject enc;
		enc.insert("bitrate", 1000000);
		arr << enc;
		enc = QJsonObject();
		enc.insert("bitrate", 4000000);
		arr << enc;
		sets.insert("encoders", arr);
		saveSettings("settings.json", sets);
	}
	FFmpegDecoder::suppressDebugMessages();
	wrap = NULL;
	ptzp = NULL;
	ptzpStatus = true;
	getScreenShot = false;
	grpcServ = new GrpcThread(50059, this);
	grpcServ->start();
	period = 0;
	previousPanValue = 0;
	previousTiltValue = 0;
	enableVideoStabilization = true;
	enableVideoTrack = true;
	triggeredByTestAPI = false;

	dayPipelineElapsed.restart();
	thPipelineElapsed.restart();

	gpiocont = new GpioController;
	gpiocont->requestGpio(GPIO_WATER, GpioController::OUTPUT);
	gpiocont->requestGpio(GPIO_WIPER, GpioController::OUTPUT);

	connect(this, SIGNAL(rebootMe(int)), SLOT(rebootSlot(int)), Qt::QueuedConnection);
}

void SmartStreamer::reboot(int seconds)
{
	emit rebootMe(seconds);
}

bool SmartStreamer::goToZeroPosition()
{
	float pan = pt->getPanAngle(); // every degree going to in about 4 milisecond for ARYA.
	int time = 0;
	if (pan >= 180)
		time = (360 - pan) * 0.04 + 1;
	else
		time = (pan - 0) * 0.04 + 1;
	pt->panTiltGoPos(0, 0);
	sleep(time);
	if (wrap) {
		if (wrap->mode == wrap->Panaroma) {
			thermalCam->setProperty(2, 0x03);
			wrap->panaroma.initStart = false;
			wrap->panaroma.tiltStartAngle = pt->getTiltAngle();
			wrap->panaroma.panStartAngle = pt->getPanAngle();
		} else if (wrap->mode == wrap->Calibration)
			wrap->calibration.initializing = true;
	}
	return true;
}

bool SmartStreamer::startSpinnig(float sSpeed)
{
	if (sSpeed == 0)
		sSpeed = 0.03;
	pt->panTiltAbs(sSpeed, 0);
	int span = 0;
	span = (int) pt->getPanAngle();
	while (span == 0)
		span = (int) pt->getPanAngle();
	if (span != 0) {
		pt->setTransportInterval(25);
		wrap->panaroma.startSpinnig = true;
		wrap->panaroma.initializing = 1;
	}
	return true;
}

void SmartStreamer::doPanaroma(const RawBuffer &buf)
{
	if (!ptzpStatus)
		return;
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
		zoom = thermalCam->getZoom();
		pan = pt->getPanAngle();
		tilt = pt->getTiltAngle();
		ffDebug() << "Panaroma doing, pan angle %f, Tilt angle %f, Zoom angle %d"<<  pan << tilt <<  zoom;
		pan_tilt_zoom_read[0] = pan;
		pan_tilt_zoom_read[1] = tilt;
		pan_tilt_zoom_read[2] = 120000;
		wrap->viaPan(buf, pan_tilt_zoom_read, wrap->panaroma.initializing);
		wrap->panaroma.initializing = 0;
		if (wrap->meta[0] != 0) {
			if (wrap->meta[3] == 1)
				wrap->panaroma.panStartAngle = wrap->meta[1] + (float)(wrap->meta[2]) / 10.0;
			else if (wrap->meta[3] == 2)
				wrap->panaroma.panStartAngle = -(wrap->meta[1] + (float)(wrap->meta[2]) / 10.0);
			else
				wrap->panaroma.panStartAngle = 0;
			pt->panTiltStop();
			wrap->stopPanaroma();
			pt->setTransportInterval(100);
		}
	}
}

void SmartStreamer::doCalibration(const RawBuffer &buf)
{
	if (!ptzpStatus)
		return;
	if (wrap->calibration.stop)
		return;
	float pan = pt->getPanAngle();
	float tilt = pt->getTiltAngle();
	if (wrap->calibration.stateCalib == 0) {
		if (!wrap->calibration.initializing) {
			thermalCam->setProperty(2, 0x03);
			goToZeroPosition();
		}
		wrap->calibration.stateCalib = wrap->viaCalibration(buf);
		if (wrap->calibration.stateCalib == 0) {
			pan = pan + 5.0;
			pt->panTiltGoPos(pan, tilt);
		}
	} else if (wrap->calibration.stateCalib == 1) {
		pan = pan + 5.0;
		pt->panTiltGoPos(pan, tilt);
		pan = pt->getPanAngle();
		wrap->calibration.init = 0;
		wrap->calibration.fovValue = 1;
		wrap->calibration.stateCalib = wrap->viaCalibration(buf);
		if (wrap->calibration.stateCalib == 0) {
			wrap->calibration.init = 0;
			wrap->calibration.fovValue = 0;
		}
	} else if(wrap->calibration.stateCalib == 3) {
		pt->panTiltStop();
		wrap->stopCalibration();
	}
	return;
}

void SmartStreamer::doMotionDetection(const RawBuffer &buf)
{
	if (!ptzpStatus)
		return;
	if (wrap->motion.stop) {
		sei->clearLastSEIMessage();
		return;
	}
	if (wrap->motion.start) {
		wrap->viaBase(buf, wrap->motion.initializing);
		wrap->motion.initializing = 0;
		if (sei) {
			QByteArray ba = QByteArray((char *)wrap->meta, 4096);
			sei->processMessage(ba);
		}
	}
}
int set_proper_zoomposs(int desired_FOVx)
{
	float lutFOV[26*3];
	lutFOV[0] = 88;lutFOV[1] = 122;lutFOV[2] = 157;lutFOV[3] = 191;lutFOV[4] = 225;lutFOV[5] = 259;lutFOV[6] = 294;lutFOV[7] = 328;
	lutFOV[8] = 362;lutFOV[9] = 397;lutFOV[10] = 431;lutFOV[11] = 465;lutFOV[12] = 499;lutFOV[13] = 534;lutFOV[14] = 568;lutFOV[15] = 602;
	lutFOV[16] = 637;lutFOV[17] = 671;lutFOV[18] = 705;lutFOV[19] = 740;lutFOV[20] = 774;lutFOV[21] = 808;lutFOV[22] = 843;lutFOV[23] = 877;
	lutFOV[24] = 911;lutFOV[25] = 945;

	lutFOV[0+26] = 25.6;lutFOV[1+26] = 22.8;lutFOV[2+26] = 20.4;lutFOV[3+26] = 18.2;lutFOV[4+26] = 16.2;lutFOV[5+26] = 14.4;lutFOV[6+26] = 12.8;lutFOV[7+26] = 11.3;
	lutFOV[8+26] = 9.9;lutFOV[9+26] = 8.8;lutFOV[10+26] = 7.7;lutFOV[11+26] = 6.7;lutFOV[12+26] = 5.9;lutFOV[13+26] = 5.1;lutFOV[14+26] = 4.4;lutFOV[15] = 3.8;
	lutFOV[16+26] = 3.3;lutFOV[17+26] = 2.8;lutFOV[18+26] = 2.4;lutFOV[19+26] = 2.1;lutFOV[20+26] = 1.7;lutFOV[21+26] = 1.5;lutFOV[22+26] = 1.3;lutFOV[23+26] = 1.1;
	lutFOV[24+26] = 0.9;lutFOV[25+26] = 0.8;

	int minind=0;
	int maxind=1;

	if(desired_FOVx>lutFOV[26])
	{
		return 88/4;
	}
	else if(desired_FOVx<lutFOV[51])
	{
		return 945/4;
	}
	else
	{
		for(int i=0;i<24;i++)
		{
			if(desired_FOVx>=lutFOV[i+1+26] && desired_FOVx<=lutFOV[i+26])
			{
				maxind=i;
				minind=i+1;
				break;
			}
		}

		float total_diff = lutFOV[maxind+26] - lutFOV[minind+26];

		float new_zoom_pos = ((lutFOV[maxind+26]-(float)desired_FOVx)*lutFOV[minind] + ((float)desired_FOVx-lutFOV[minind+26])*lutFOV[maxind]) / total_diff;

		return (int)(new_zoom_pos/4);
	}
}
void interpolate_FOVs(int zoom_pos,float fovs[])
{
	float lutFOV[26*3];
	lutFOV[0] = 88;lutFOV[1] = 122;lutFOV[2] = 157;lutFOV[3] = 191;lutFOV[4] = 225;lutFOV[5] = 259;lutFOV[6] = 294;lutFOV[7] = 328;
	lutFOV[8] = 362;lutFOV[9] = 397;lutFOV[10] = 431;lutFOV[11] = 465;lutFOV[12] = 499;lutFOV[13] = 534;lutFOV[14] = 568;lutFOV[15] = 602;
	lutFOV[16] = 637;lutFOV[17] = 671;lutFOV[18] = 705;lutFOV[19] = 740;lutFOV[20] = 774;lutFOV[21] = 808;lutFOV[22] = 843;lutFOV[23] = 877;
	lutFOV[24] = 911;lutFOV[25] = 945;

	lutFOV[0+26] = 25.6;lutFOV[1+26] = 22.8;lutFOV[2+26] = 20.4;lutFOV[3+26] = 18.2;lutFOV[4+26] = 16.2;lutFOV[5+26] = 14.4;lutFOV[6+26] = 12.8;lutFOV[7+26] = 11.3;
	lutFOV[8+26] = 9.9;lutFOV[9+26] = 8.8;lutFOV[10+26] = 7.7;lutFOV[11+26] = 6.7;lutFOV[12+26] = 5.9;lutFOV[13+26] = 5.1;lutFOV[14+26] = 4.4;lutFOV[15] = 3.8;
	lutFOV[16+26] = 3.3;lutFOV[17+26] = 2.8;lutFOV[18+26] = 2.4;lutFOV[19+26] = 2.1;lutFOV[20+26] = 1.7;lutFOV[21+26] = 1.5;lutFOV[22+26] = 1.3;lutFOV[23+26] = 1.1;
	lutFOV[24+26] = 0.9;lutFOV[25+26] = 0.8;

	lutFOV[0+52] = 15;lutFOV[1+52] = 13.3;lutFOV[2+52] = 12;lutFOV[3+52] = 10.7;lutFOV[4+52] = 9.6;lutFOV[5+52] = 8.5;lutFOV[6+52] = 7.6;lutFOV[7+52] = 6.7;
	lutFOV[8+52] = 6;lutFOV[9+52] = 5.3;lutFOV[10+52] = 4.7;lutFOV[11+52] = 4.1;lutFOV[12+52] = 3.6;lutFOV[13+52] = 3.2;lutFOV[14+52] = 2.8;lutFOV[15+52] = 2.5;
	lutFOV[16+52] = 2.2;lutFOV[17+52] = 1.9;lutFOV[18+52] = 1.6;lutFOV[19+52] = 1.4;lutFOV[20+52] = 1.2;lutFOV[21+52] = 1;lutFOV[22+52] = 0.9;lutFOV[23+52] = 0.7;
	lutFOV[24+52] = 0.6;lutFOV[25+52] = 0.46;

	int minind=0;
	int maxind=1;

	if(zoom_pos<=lutFOV[0])
	{
		fovs[0] = lutFOV[26];
		fovs[1] = lutFOV[52];
	}
	else if(zoom_pos>=lutFOV[25])
	{
		fovs[0] = lutFOV[51];
		fovs[1] = lutFOV[77];
	}
	else
	{
		for(int i=0;i<25;i++)
		{
			if(zoom_pos<=lutFOV[i+1] && zoom_pos>=lutFOV[i])
			{
				minind=i;
				maxind=i+1;
				break;
			}
		}

		float total_diff = lutFOV[maxind] - lutFOV[minind];

		fovs[0] = ((lutFOV[maxind]-zoom_pos)*lutFOV[minind+26] + (zoom_pos-lutFOV[minind])*lutFOV[maxind+26]) / total_diff;
		fovs[1] = ((lutFOV[maxind]-zoom_pos)*lutFOV[minind+52] + (zoom_pos-lutFOV[minind])*lutFOV[maxind+52]) / total_diff;
	}
}
int set_proper_zoomposs_Thermal(int desired_FOVx)
{
	float lutFOV[16];
	lutFOV[0] = 31249;lutFOV[1] = 27656;lutFOV[2] = 24375;lutFOV[3] = 20625;lutFOV[4] = 17500;lutFOV[5] = 13750;lutFOV[6] = 10625;lutFOV[7] = 6875;
	lutFOV[8] = 9.38;lutFOV[9] = 5.28;lutFOV[10] = 3.19;lutFOV[11] = 1.46;lutFOV[12] = 1.42;lutFOV[13] = 1.25;lutFOV[14] = 0.92;lutFOV[15] = 0.82;

	int minind=0;
	int maxind=1;

	if(desired_FOVx>lutFOV[8])
	{
		return 31249/150;//map to unsigned char
	}
	else if(desired_FOVx<lutFOV[15])
	{
		return 6875/150;//map to unsigned char
	}
	else
	{
		for(int i=0;i<7;i++)
		{
			if(desired_FOVx>=lutFOV[i+1+8] && desired_FOVx<=lutFOV[i+8])
			{
				maxind=i;
				minind=i+1;
				break;
			}
		}

		float total_diff = lutFOV[maxind+8] - lutFOV[minind+8];

		float new_zoom_pos = ((lutFOV[maxind+8]-(float)desired_FOVx)*lutFOV[minind] + ((float)desired_FOVx-lutFOV[minind+8])*lutFOV[maxind]) / total_diff;

		return (int)(new_zoom_pos/150);
	}
}
void interpolate_FOVs_Thermal(int zoom_pos,float fovs[])
{
	float lutFOV[24];
	lutFOV[0] = 31249;lutFOV[1] = 27656;lutFOV[2] = 24375;lutFOV[3] = 20625;lutFOV[4] = 17500;lutFOV[5] = 13750;lutFOV[6] = 10625;lutFOV[7] = 6875;
	lutFOV[8] = 9.38;lutFOV[9] = 5.28;lutFOV[10] = 3.19;lutFOV[11] = 1.46;lutFOV[12] = 1.42;lutFOV[13] = 1.25;lutFOV[14] = 0.92;lutFOV[15] = 0.82;
	lutFOV[16] = 7.44;lutFOV[17] = 3.98;lutFOV[18] = 2.25;lutFOV[19] = 1.28;lutFOV[20] = 1.08;lutFOV[21] = 0.93;lutFOV[22] = 0.75;lutFOV[23] = 0.66;

	int minind=0;
	int maxind=1;

	if(zoom_pos<=lutFOV[7])
	{
		fovs[0] = lutFOV[15];
		fovs[1] = lutFOV[23];
	}
	else if(zoom_pos>=lutFOV[0])
	{
		fovs[0] = lutFOV[8];
		fovs[1] = lutFOV[16];
	}
	else
	{
		for(int i=0;i<7;i++)
		{
			if(zoom_pos>=lutFOV[i+1] && zoom_pos<=lutFOV[i])
			{
				minind=i;
				maxind=i+1;
				break;
			}
		}

		float total_diff = lutFOV[minind] - lutFOV[maxind];

		fovs[0] = ((lutFOV[minind]-zoom_pos)*lutFOV[maxind+8] + (zoom_pos-lutFOV[maxind])*lutFOV[minind+8]) / total_diff;
		fovs[1] = ((lutFOV[minind]-zoom_pos)*lutFOV[maxind+16] + (zoom_pos-lutFOV[maxind])*lutFOV[minind+16]) / total_diff;
	}
}
void SmartStreamer::doDirectTrack(const RawBuffer &buf, ViaWrapper *wrap)
{
	if (wrap->track.stop)
		return;
	if(wrap->track.start && !enableVideoTrack) {
		wrap->stopTrack();
		return;
	}
	if (wrap->track.start) {
		panValue = pt->getPanAngle();
		tiltValue = pt->getTiltAngle();
		enableVideoStabilization = false;
		if (wrap->track.init == 1) {
//			float pan_tilt_zoom_read[6];
			wrap->pan_tilt_zoom_read[0] = panValue;
			wrap->pan_tilt_zoom_read[1] = tiltValue;
			//printf("degress: %f %f\n",panValue,tiltValue);
			float fovs[2];
			if (wrap->deviceType == 1) {
				interpolate_FOVs(ptzp->getHead(0)->getZoom(),fovs);
			} else if (wrap->deviceType == 2) {
				interpolate_FOVs_Thermal(ptzp->getHead(2)->getZoom(),fovs);
			}
			wrap->pan_tilt_zoom_read[3] = fovs[0];
			wrap->pan_tilt_zoom_read[4] = fovs[1];

			wrap->viaTrack(buf);
			wrap->track.init = 0;
		}
		else if (wrap->track.init == 0 && period > 0) {
			float fovs[2];
			wrap->pan_tilt_zoom_read[0] = panValue;
			wrap->pan_tilt_zoom_read[1] = tiltValue;

			if (wrap->deviceType == 1) {
				interpolate_FOVs(ptzp->getHead(0)->getZoom(),fovs);
			} else if (wrap->deviceType == 2) {
				interpolate_FOVs_Thermal(ptzp->getHead(2)->getZoom(),fovs);
			}
			wrap->pan_tilt_zoom_read[3] = fovs[0];
			wrap->pan_tilt_zoom_read[4] = fovs[1];
			qDebug() << "FOVs:" << fovs[0] << fovs[1];
			wrap->viaTrack(buf);

			// Zoom adaptation
			if (wrap->deviceType == 1)//TV
			{
				if(wrap->meta[32]>0)//time to change the zoom pos
				{
					int new_zoom_pos = set_proper_zoomposs((int)wrap->meta[32]);
					wrap->meta[32]   = new_zoom_pos;
				}
				else
				{
					wrap->meta[32] = 0;
				}
			}
			else if (wrap->deviceType == 2)//Thermal
			{
				if(wrap->meta[32]>0)//time to change the zoom pos
				{
					int new_zoom_pos = set_proper_zoomposs_Thermal((int)wrap->meta[32]);
					wrap->meta[32]   = new_zoom_pos;
				}
				else
				{
					wrap->meta[32] = 0;
				}
			}

			if (((wrap->meta[31] < wrap->track.score) && wrap->track.score>0) || wrap->meta[31]<0)
			{
				wrap->stopTrack();
				pt->panTiltStop();
//				wrap->startStabilization();
				enableVideoStabilization = true;
			}
			if (period>=wrap->track.interval && wrap->track.interval>0)
			{
				wrap->stopTrack();
				pt->panTiltStop();
				//wrap->startStabilization();
				enableVideoStabilization = true;
			}
		}
		if (wrap->track.start) {
			period ++;
			if(period % 5 == 0)
				setPtz(wrap->meta, wrap);
			/*
			else if(period%4==3)// in case of zoom adaptation
			{
				if (wrap->deviceType == 1)
					ptzp->getHead(0)->setProperty(16,2);
				if (wrap->deviceType == 2)
					ptzp->getHead(2)->setProperty(16,2);
			}*/
		}
	}
}
void SmartStreamer::doStabilization(const RawBuffer &buf, ViaWrapper *wrap)
{
	//ffDebug() << "Inside the stabilization module " << wrap;
	//return;
	if(wrapDayTv->track.start || wrapThermal->track.start)
		return;
	if (!enableVideoStabilization) {
		//ffDebug() << "there exist move";
		if (!wrap->stabil.stop) {
			wrap->stopStabilization();
			//ffDebug() << "releasing stabilization";
		}
		return;
	} else {
		if (wrap->stabil.stop && triggeredByTestAPI) {
			//ffDebug() << "re-initializing stabilization";
			wrap->startStabilization();
		} else if (wrap->stabil.start){
			wrap->stabil.counter++;
			if(wrap->stabil.counter > 75) {
				//ffDebug() << "stabilization is initialized << " << buf.constPars()->videoHeight << buf.constPars()->videoWidth << buf.size();
				wrap->viaByPass(buf);
				wrap->stabil.initializing = 0;
			}
		}
	}
	return;
}

struct TbgthData
{
	class PipelineCommon {
	public:
		void setup(SmartStreamer *streamer, const QString &rtspUrl, int frameRate, FunctionPipeElement *pipeel, bool softEncoder, int bitrate)
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

		SmartStreamer::Parameters pars;
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

int SmartStreamer::setupTbgthCombined(const QString &rtspUrl1, const QString &rtspUrl2)
{
	printParameters();

	TbgthData *tbgth = new TbgthData;
	tbgth->daytv.pars = pars;
	tbgth->daytv.setup(this, rtspUrl1, 15, newFunctionPipe(SmartStreamer, this, SmartStreamer::processMainYUV), false,
					   sets["encoders"].toArray().at(1).toObject()["bitrate"].toInt());
	tbgth->thermal.pars = pars2;
	tbgth->thermal.setup(this, rtspUrl2, 25, newFunctionPipe(SmartStreamer, this, SmartStreamer::processMainYUVThermal), true,
						 sets["encoders"].toArray().at(0).toObject()["bitrate"].toInt());

	rtspServer = new BaseRtspServer(this, 8554);
	addRtspServer(tbgth->daytv.rtpout, tbgth->daytv.rtpoutvs, 1);
	addRtspServer(tbgth->thermal.rtpout, tbgth->thermal.rtpoutvs, 2);

	return 0;
}

int SmartStreamer::setupRtspClient(const QString &rtspUrl)
{
	printParameters();

	rtp = new RtpReceiver(this);
	rtp->useThreadedReading(true);
	rtpout = new RtpTransmitter(this);
	rtpout->forwardRtpTs(true);
	rtpout->setRtcp(false);
	rtpout->setH264SEIInsertion(true);
	rtpout->useIncomingTimestamp(true);
	//rtpout->setMaximumPayloadSize(1400);
	//rtpout->setTrafficShaping(true, 4000000, 1, 10);

	/* queues will be used to de-multiplex streams */
	BufferQueue *queue = new BufferQueue;
	BufferQueue *queueScalerEngine = new BufferQueue;
	//BufferQueue *queueNv12 = new BufferQueue;

	//vout = new QtVideoOutput;
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

//	VideoScaler *rgbConvNv12 = new VideoScaler;
//	rgbConvNv12->setOutputFormat(AV_PIX_FMT_NV12);
//	rgbConvNv12->setOutputResolution(768,432);
//	rgbConvNv12->setMode(1);

//	QString device = "/dev/video3";
//	V4l2Output *v4l2 = new V4l2Output();
//	v4l2->setParameter("videoWidth", 768);
//	v4l2->setParameter("videoHeight", 432);
//	v4l2->setParameter("device", device);

	BaseLmmPipeline *p1 = addPipeline();
	p1->append(rtp);
	p1->append(queue);
	p1->append(dec);
	//p1->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::checkPoint));
	p1->append(queueScalerEngine);
	if (pars.pipelineFlags & Parameters::EL_YUV_PROCESS)
		p1->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::processMainYUV));
//	p1->append(rgbConvNv12);
//	p1->append(v4l2);
	RtpTransmitter *rtpout2 = NULL;
	if (1) {
		x264Encoder *enc = new x264Encoder;
		enc->setVideoResolution(QSize(pars.decWidth, pars.decHeight));
		enc->setBitrate(4000000);
		enc->setThreadCount(2);
		enc->setPreset("faster");
		//p1->append(enc);

		TX1VideoEncoder *enc2 = new TX1VideoEncoder;
		p1->append(enc2);

		rtpout2 = new RtpTransmitter(this);
		rtpout2->forwardRtpTs(false);
		rtpout2->setRtcp(false);
		rtpout2->setH264SEIInsertion(false);
		rtpout2->useIncomingTimestamp(false);
		rtpout2->setUseAbsoluteTimestamp(false);
		rtpout2->setFrameRate(25);
		p1->append(rtpout2);
	}
	p1->end();
	if (pars.rtpBufferDuration) {
		rtp->getOutputQueue(0)->setBufferDuration(pars.rtpBufferDuration);
		rtp->getOutputQueue(0)->setTimestampingMethod(ElementIOQueue::TS_DURATION);
	}
	if (pars.decOutputInFps > 0 && pars.decOutputOutFps > 0) {
		dec->getOutputQueue(0)->setRateReduction(pars.decOutputInFps, pars.decOutputOutFps);
	}

//	V4l2Input *v4l2Input = new V4l2Input();
//	v4l2Input->setParameter("device", device);
//	v4l2Input->setParameter("videoWidth", 768);
//	v4l2Input->setParameter("videoHeight", 432);

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
//	if (pars.pipelineFlags & Parameters::EL_QT_VOUT)
//		p3->append(vout);
	p3->append(rgbScaler);
	if (pars.pipelineFlags & Parameters::EL_RGBS_PROCESS)
		p3->append(newFunctionPipe(SmartStreamer, this, SmartStreamer::processScaledRGB));
	if (pars.pipelineFlags & Parameters::EL_MJPEG_OUTPUT)
	//	p3->append(mjpeg);
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
	rtsp->setDefaultRtpTrack(rtp);
	//rtsp->addSetupTrack("videoTrack", rtp);
	//rtsp->addSetupTrack("trackID=0", rtp);
	//rtsp->addSetupTrack("video", rtp);
	if (!pars.rtspClientUser.isEmpty())
		rtsp->setAuthCredentials(pars.rtspClientUser, pars.rtspClientPass);//"admin", "moxamoxa"
	if (rtspUrl.startsWith("rtsp://"))
		rtsp->setServerUrl(QString("%1").arg(rtspUrl));
	else
		rtsp->setServerUrl(QString("rtsp://%1").arg(rtspUrl));
	if (pars.enableMoxaHacks)
		rtsp->setMoxaHacks(true);

	rtspServer = new BaseRtspServer(this, 8554);
	//rtspServer->setNetworkInterface("eth1");
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
	return 0;
}

void SmartStreamer::setupVideoAnalysis()
{

	ffDebug() << "wrap will be initialized";
#ifdef HAVE_VIA_WRAPPER
	wrapDayTv = new ViaWrapper(1);
	wrap = wrapDayTv;
	wrapThermal = new ViaWrapper(2);
#endif
//	if (wrapDayTv)
//		wrapDayTv->startStabilization();
//	if (wrapThermal)
//		wrapThermal->startStabilization();
	ffDebug() << "wrap was initialized";

//thermalCam->setProperty(4,512);
	//thermalCam->setProperty(16,2);
}

void SmartStreamer::setupPanTiltZoomDriver(const QString &target)
{
	ptzp = new TbgthDriver(true);
	if (ptzp->setTarget(target) != 0)
		return;
	ffDebug() << "Pan tilt sockets were set";
//	ptzp->startSocketApi(8945);
	ptzp->startGrpcApi(50058);
	pt = ptzp->getHead(1);
	thermalCam = ptzp->getHead(0);
	ptzp->setPatternHandler(new PtzNotification(this));
	PtzpDriver::SpeedRegulation sreg = ptzp->getSpeedRegulation();
	sreg.enable = true;
	sreg.ipol = PtzpDriver::SpeedRegulation::LINEAR;
	sreg.minSpeed = 0.01;
	sreg.minZoom = 88;
	sreg.maxZoom = 945;
	sreg.zoomHead = ptzp->getHead(0);
	ptzp->setSpeedRegulation(sreg);

	ffDebug() << "going into setupVideoAnalysis";
	setupVideoAnalysis();
	//thermalCam->setZoom(230);

	return;
}

int SmartStreamer::processMainYUV(const RawBuffer &buf)
{
	dayPipelineElapsed.restart();
	if (PRINT_BUFS)
		ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
				  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
#ifdef HAVE_VIA_WRAPPER
	if (!wrapDayTv)
		return 0;
	QMutexLocker ml(&viaLock);

	doStabilization(buf,wrapDayTv);
	doDirectTrack(buf, wrapDayTv);
#endif
	return 0;
}

int SmartStreamer::processMainYUVThermal(const RawBuffer &buf)
{
	thPipelineElapsed.restart();
	if (PRINT_BUFS)
		ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
				  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
#ifdef HAVE_VIA_WRAPPER
	if (!wrapThermal)
		return 0;
	QMutexLocker ml(&viaLock);
	doStabilization(buf,wrapThermal);
	doDirectTrack(buf, wrapThermal);
#endif
	return 0;
}

int SmartStreamer::processMainRGB(const RawBuffer &buf)
{
	if (PRINT_BUFS)
		ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
				  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
	mutex.lock();
	screenBuf = buf;
	mutex.unlock();
	return 0;
}

int SmartStreamer::processScaledRGB(const RawBuffer &buf)
{
	if (PRINT_BUFS)
		ffDebug() << buf.getMimeType() << buf.size() << FFmpegColorSpace::getName(buf.constPars()->avPixelFormat)
				  << buf.constPars()->videoWidth << buf.constPars()->videoHeight;
	mutex.lock();
	screenSecBuf = buf;
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

int SmartStreamer::setPtz(uchar meta[], ViaWrapper *wrap)
{
	int sei_direction = meta[10];
	float pan_speed = ((float)meta[11]) / 3217.0;
	float tilt_speed = ((float)meta[12]) / 2680.0;
	if (pt == NULL)
		return -1;

	if (sei_direction == 0) {
		pt->panTiltStop();
	} else if (sei_direction == 2)
	{
		pt->panRight(pan_speed);
	} else if (sei_direction == 4) {
		//pan_speed = -1 * pan_speed;
		pt->panLeft(pan_speed);
		//ffDebug() << "pan left is invoked";
	} else if (sei_direction == 16) {
		pt->tiltDown(tilt_speed);
	} else if (sei_direction == 8) {
		//tilt_speed = -1 * tilt_speed;
		pt->tiltUp(tilt_speed);
	} else if (sei_direction == 18){
		pt->panTiltAbs(pan_speed,tilt_speed);
	} else if (sei_direction == 20){
		pan_speed = -1 * pan_speed;
		pt->panTiltAbs(pan_speed,tilt_speed);
	} 	else if (sei_direction == 10){
		tilt_speed = -1 * tilt_speed;
		pt->panTiltAbs(pan_speed,tilt_speed);
	}
	else if (sei_direction == 12){
		pan_speed = -1 * pan_speed;
		tilt_speed = -1 * tilt_speed;
		pt->panTiltAbs(pan_speed,tilt_speed);
	}
	else
		pt->panTiltStop();

	/*
	if(wrap->meta[32]>0 && wrap->deviceType==1)
		ptzp->getHead(0)->setZoom(4*wrap->meta[32]); //set zoom TV
	else if(wrap->meta[32]>0 && wrap->deviceType==2)
		ptzp->getHead(2)->setZoom(150*wrap->meta[32]); //set zoom Thermal
	*/

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

void SmartStreamer::ptzCommandRecved(int cmd)
{
	(void)cmd;
	if (!wrap)
		return;
	qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << cmd << wrap << wrapDayTv << wrapThermal << wrap->track.start;
	QMutexLocker ml(&viaLock);
	//zoom stop -> 6     pan-tilt -> stop
	if (cmd == 7 || cmd == 6)
	{
		//if(wrap->track.stop)
			enableVideoStabilization = true;
			enableVideoTrack = true;
	}
	else {
		enableVideoStabilization = false;
		if (wrap->track.start) {
			ffDebug() << "-----------------------------------------------wowowowoowowowowoowow";
			wrap->stopTrack();
			if (cmd == 4 || cmd == 5)
				pt->panTiltStop();
			enableVideoTrack = false;
		}
	}
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

void SmartStreamer::rebootSlot(int seconds)
{
	sleep(seconds);
	exit(123);
}

grpc::Status SmartStreamer::SetCurrentMode(grpc::ServerContext *context, const OrionCommunication::ModeQ *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	Q_UNUSED(response)
	// unusing function.
	return grpc::Status::OK;
}

grpc::Status SmartStreamer::GetCurrentMode(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::ModeQ *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	if (!wrap)
		return Status::OK;
	ViaWrapper::ModeState myMode = wrap->getMode();

	if (ViaWrapper::Panaroma == myMode)
		response->set_mode(response->PANAROMA);
	else if (ViaWrapper::Motion == myMode)
		response->set_mode(response->MOTION_DETECTION);
	else if (ViaWrapper::Track == myMode)
		response->set_mode(response->TRACK);
	else
		response->set_mode(response->NONE);
	return grpc::Status::OK;
}

grpc::Status SmartStreamer::SetVideoOverlay(grpc::ServerContext *context, const OrionCommunication::OverlayQ *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	QString config = QString("ctoken=osdcfg0%1&action=setconfig").arg(request->configno());
	QString type = "type=0";
	QString display = QString("display=%1").arg(request->display());
	QString position;
	if (request->pos() == OrionCommunication::OverlayQ::Custom) {
		position = QString("position=4&posx=%1&posy=%2").arg(request->posx()).arg(request->posy());
	} else {
		position = QString("position=%1&posx=0&posy=0").arg(request->pos());
	}
	QString bgspan = "bgspan=0";
	QString textSize = QString("textsize=%1").arg(request->textsize());
	QString dateTimeFormat = QString("datetimeformat=%1").arg(request->datetimeformat());
	QString showDate = QString("showdate=%1").arg(request->displaydate());
	QString showTime = QString("showtime=%1").arg(request->displaytime());
	QString text = QString("text=%1").arg(QString::fromStdString(request->text()));
	QString overlayData = config + "&"+
			type + "&" +
			display + "&" +
			position + "&" +
			bgspan + "&" +
			textSize + "&" +
			dateTimeFormat + "&" +
			showDate + "&" +
			showTime + "&" +
			text;
//	ptzp->setOverlay(overlayData);
	response->set_err(0);
	return Status::OK;
}

grpc::Status SmartStreamer::SetPanaromaParameters(grpc::ServerContext *context, const OrionCommunication::PanoramaPars *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(response)
	if (!wrap)
		return Status::OK;
	qDebug() << request->fov();
	return grpc::Status::OK;
}

grpc::Status SmartStreamer::GetPanaromaFrames(grpc::ServerContext *context, const OrionCommunication::FrameModeQ *request, ::grpc::ServerWriter<OrionCommunication::PanoramaFrame> *writer)
{
	int state = 0;
	int sleepint = 10 * 1000;
	int currentProgress = 0;
	int progressTotal = 20;
	QStringList messages;
	messages << QString("Gunduz kamera haberlesme testi - %1 / %2");
	messages << QString("Pan-tilt kamera haberlesme testi - %1 / %2");
	messages << QString("Termal kamera haberlesme testi - %1 / %2");
	messages << QString("Gunduz kamera goruntu testi - %1 / %2");
	messages << QString("Termal kamera goruntu testi - %1 / %2");
	while (1) {
		usleep(sleepint);
		if (currentProgress++ >= progressTotal) {
			currentProgress = 0;
			state++;
			if (state > 4)
				break;
		}
		QString mes = messages[state].arg(currentProgress).arg(progressTotal);
		OrionCommunication::PanoramaFrame panFrames;
		panFrames.set_valid(true);
		panFrames.set_progress((currentProgress + (state * progressTotal)) / (5 * progressTotal));
		panFrames.set_framedata(mes.toStdString().data());
		writer->Write(panFrames);
	}
	OrionCommunication::PanoramaFrame panFrames;
	panFrames.set_valid(true);
	panFrames.set_progress((currentProgress + (state * progressTotal)) / (5 * progressTotal));
	panFrames.set_framedata(QString("%1,%2,%3,%4,%5")
							.arg(ptzp->getHead(0)->communicationElapsed() < 1000 ? "1" : "0")
							.arg(ptzp->getHead(1)->communicationElapsed() < 1000 ? "1" : "0")
							.arg(ptzp->getHead(2)->communicationElapsed() < 1000 ? "1" : "0")
							.arg(dayPipelineElapsed.elapsed() < 1000 ? "1" : "0")
							.arg(thPipelineElapsed.elapsed() < 1000 ? "1" : "0").toStdString().data()
							);
	writer->Write(panFrames);
	return Status::OK;
#if 0
	Q_UNUSED(context)
	if (!wrap)
		return Status::OK;
	if (wrap->getMode() != ViaWrapper::Panaroma)
		return Status::CANCELLED;
	bool lastFrame;
	if (request->mode() == 0)
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
#endif
}

grpc::Status SmartStreamer::RunPanaroma(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
	return Status::OK;
	qDebug() << "Run auto track is invoked";
	Q_UNUSED(context)
	if (!QFile::exists("track_alg_parameters.txt") || !QFile::exists("track_parameters.txt")) {
		mDebug("The .txt files can't found. This program cancelled.");
		return Status::CANCELLED;
	}
	if (!wrap) {
		mDebug("Helper ViaWrapper class uninitalized.");
		return Status::CANCELLED;
	}
	//OrionCommunication::AutoTrackQ::Mode mode;
	//mode = request->mode();
	wrap->track.score = 0;//request->trackscore();
	wrap->track.interval = 0;//request->trackinterval();
	//if (mode == OrionCommunication::AutoTrackQ::DistanceToCenter) {
	//	wrap->track.mode = STrack::Mode::DistanceToCenter;
	//} else if (mode == OrionCommunication::AutoTrackQ::SizeOfObject) {
	//	wrap->track.mode = STrack::Mode::SizeOfObject;
	//} else if (mode == OrionCommunication::AutoTrackQ::UserMode) {
	//	wrap->track.mode = STrack::Mode::UserMode;
	//	wrap->track.objWidth = request->object().width();
	//	wrap->track.objHeight = request->object().height();
	//	wrap->track.objPointx = request->object().point_x();
	//	wrap->track.objPointy = request->object().point_y();
	//} else {
	//	mDebug("Mode request failed");
	//	return Status::CANCELLED;
	//}
	wrap->startTrack();
	qDebug() << "startTrack is invoked";
	return Status::OK;

//	Q_UNUSED(context)
//	Q_UNUSED(request)
//	Q_UNUSED(response)
//	if (!wrap)
//		return Status::OK;
//	if (!QFile::exists("pan_params.txt") || !QFile::exists("alg_parameters.txt")) {
//		mDebug("The .txt files cant found. This program cancelled.");
//		return Status::CANCELLED;
//	}
//	QProcess::execute("bash -c \"rm -f /home/ubuntu/Desktop/Pan_images/*\"");
//	wrap->startPanaroma();
//	return Status::OK;


}

grpc::Status SmartStreamer::RunMotion(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	if (!wrap)
		return Status::OK;
	if (!QFile::exists("pan_params.txt") || !QFile::exists("alg_parameters.txt")) {
		mDebug("The .txt files cant found. This program cancelled.");
		return Status::CANCELLED;
	}
	wrap->startMotion();
	response->set_err(0);
	return Status::OK;
}

grpc::Status SmartStreamer::StopPanaroma(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	if (!wrap)
		return Status::OK;
	wrap->stopPanaroma();
	pt->panTiltStop();
	pt->setTransportInterval(100);
	response->set_err(0);
	return Status::OK;
}

grpc::Status SmartStreamer::StopMotion(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	if (!wrap)
		return Status::OK;
	wrap->stopMotion();
	if (sei)
		sei->clearLastSEIMessage();
	response->set_err(0);
	return Status::OK;
}

grpc::Status SmartStreamer::GetSecScreenShot(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::ScreenFrame *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	Q_UNUSED(response)
	mutex.lock();
	QByteArray ss = doScreenShot(screenSecBuf);
	if (ss.isEmpty())
		return Status::CANCELLED;
	response->set_frame(ss, ss.size());
	mutex.unlock();
	return Status::OK;
}

grpc::Status SmartStreamer::GetMainScreenShot(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::ScreenFrame *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	Q_UNUSED(response)
	mutex.lock();
	QByteArray ss = doScreenShot(screenBuf);
	if (ss.isEmpty())
		return Status::CANCELLED;
	response->set_frame(ss, ss.size());
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
	if (!wrap)
		return Status::OK;
	response->set_sensivity(wrap->motion.sensivity);
	return Status::OK;
}

grpc::Status SmartStreamer::SetSensivityParameter(grpc::ServerContext *context, const OrionCommunication::SetSensivity *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(response)
	if (!wrap)
		return Status::OK;
	wrap->motion.sensivity = request->sensivity();
	response->set_err(0);
	return Status::OK;
}

grpc::Status SmartStreamer::GotoPanaromaPixel(grpc::ServerContext *context, const OrionCommunication::TPoint *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	Q_UNUSED(response)
	if (!wrap)
		return Status::CANCELLED;
	if (wrap->mode != ViaWrapper::None)
		return Status::CANCELLED;
	float shiftAmount = 4.5;
	float angle = (request->x() * (360 + shiftAmount) + wrap->panaroma.panStartAngle);
	if(angle > 360)
		angle -= 360;
	if(angle < 0)
		angle += 360;
	angle = angle / 360;
	float point_x = angle;

	float fovValue = 7.5;
	float theta = fovValue * 576.0 / 720.0;

	float point_y = wrap->panaroma.tiltStartAngle - ((((float)request->y() - 0.5) / 0.5) * theta / 2.0);
	if (point_y > 45.0)
		point_y = 45.0;
	if (point_y < -45.0)
		point_y = -45.0;
	point_y = point_y;
	pt->panTiltGoPos(point_x * 360, point_y);
	return Status::OK;
}

grpc::Status SmartStreamer::RunCalibration(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	Q_UNUSED(response)
	if (!QFile::exists("pan_params.txt") || !QFile::exists("alg_parameters.txt")) {
		mDebug("The .txt files can't found. This program cancelled.");
		return Status::CANCELLED;
	}
	if (!wrap)
		return Status::OK;
	pt->setTransportInterval(50);
	wrap->startCalibration();
	return Status::OK;
}

grpc::Status SmartStreamer::StopCalibration(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	Q_UNUSED(request)
	if (!wrap)
		return Status::OK;
	wrap->stopCalibration();
	pt->panTiltStop();
	pt->setTransportInterval(100);
	response->set_err(0);
	return Status::OK;
}

grpc::Status SmartStreamer::RunStabilization(grpc::ServerContext *context, const OrionCommunication::DevicedBasedInfo *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context);
	QMutexLocker ml(&viaLock);
	qDebug() << "Start is initialized";
	OrionCommunication::DevicedBasedInfo::TV;
	if (request->device() == OrionCommunication::DevicedBasedInfo::TV)
	{
		if (!wrapDayTv)
			return Status::CANCELLED;
		wrapDayTv->startStabilization();
		if (wrapThermal)
			wrapThermal->startStabilization();
	} else if (request->device() == OrionCommunication::DevicedBasedInfo::Thermal)
	{
		if (!wrapThermal)
			return Status::CANCELLED;
		wrapThermal->startStabilization();
		if (wrapDayTv)
			wrapDayTv->startStabilization();
	}
	triggeredByTestAPI = true;
	return Status::OK;
}

grpc::Status SmartStreamer::StopStabilization(grpc::ServerContext *context, const OrionCommunication::DevicedBasedInfo *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context);
	Q_UNUSED(request);
	QMutexLocker ml(&viaLock);
	if (wrapDayTv->mode != ViaWrapper::Stabilization && wrapThermal->mode != ViaWrapper::Stabilization) {
		qDebug() << "Stop is not available";
		return Status::CANCELLED;

	}
	triggeredByTestAPI = false;
	wrapDayTv->stopStabilization();
	wrapThermal->stopStabilization();
	qDebug() << "Stabilization is stopped";
	return Status::OK;
}

grpc::Status SmartStreamer::GetUptime(grpc::ServerContext *context, const OrionCommunication::DevicedBasedInfo *request, OrionCommunication::ReturnUptimeValue *response)
{
	Q_UNUSED(context);
	qDebug() << "up time is " << SystemInfo::getUptime();
	if (request->device() == OrionCommunication::DevicedBasedInfo::TV)
		response->set_uptimevalue(SystemInfo::getUptime());
	else if (request->device() == OrionCommunication::DevicedBasedInfo::Thermal)
		response->set_uptimevalue(SystemInfo::getUptime());
	return Status::OK;
}

static float speedRegulateDay(float speed, int zoom)
{
	float fovs[2];
	interpolate_FOVs(zoom, fovs);

	int fov = fovs[0];
	if (fov < 0.8)
		return 0.01;
	if (fov > 25.6)
		return 1.0;
	float zoomr = (fov - 0.8) / (float)(25.6 - 0.8);
	speed *= zoomr;
	if (speed < 0.1)
		speed = 0.1;
	if (speed > 1.0)
		speed = 1.0;
	qDebug() << "day speed" << speed << zoom;
	return speed;
}

static float speedRegulateThermal(float speed, int zoom)
{
	float fovs[2];
	interpolate_FOVs_Thermal(zoom, fovs);

	int fov = fovs[0];
	if (fov < 0.82)
		return 0.01;
	if (fov > 9.38)
		return 1.0;
	float zoomr = (fov - 0.82) / (float)(9.38 - 0.82);
	speed *= zoomr;
	if (speed < 0.1)
		speed = 0.1;
	if (speed > 1.0)
		speed = 1.0;
	qDebug() << "thermal speed" << speed << zoom;
	return speed;
}

grpc::Status SmartStreamer::GetFovValue(grpc::ServerContext *context, const OrionCommunication::DevicedBasedInfo *request, OrionCommunication::ReturnFOVValue *response)
{
	Q_UNUSED(context);
	short fovh = 0, fovv = 0;
	float fovs[2];
	if (request->device() == OrionCommunication::DevicedBasedInfo::TV) {
		interpolate_FOVs(ptzp->getHead(0)->getZoom(), fovs);
	} else if (request->device() == OrionCommunication::DevicedBasedInfo::Thermal) {
		interpolate_FOVs_Thermal(ptzp->getHead(2)->getZoom(), fovs);
	}
	fovh = fovs[0] * 100;
	fovv = fovs[1] * 100;
	qDebug() << "fov value is " << (fovh + fovv * (int) 65536);
	response->set_fovvalue(fovs[0]);// + fovv * (int)65536);

	if (1) {
#if 0
		PtzpDriver::SpeedRegulation sreg = ptzp->getSpeedRegulation();
		sreg.enable = true;
		sreg.ipol = PtzpDriver::SpeedRegulation::LINEAR;
		sreg.minSpeed = 0.01;
		int head = 0;
		if (request->device() == OrionCommunication::DevicedBasedInfo::TV) {
			sreg.minZoom = 88;
			sreg.maxZoom = 945;
			head = 0;
		} else {
			head = 2;
			sreg.minZoom = 31249; //6875
			sreg.maxZoom = 6875; // 31249
		}
		sreg.zoomHead = ptzp->getHead(head);
		ptzp->setSpeedRegulation(sreg);
#else
		PtzpDriver::SpeedRegulation sreg = ptzp->getSpeedRegulation();
		sreg.enable = true;
		sreg.ipol = PtzpDriver::SpeedRegulation::CUSTOM;
		int head = 0;
		if (request->device() == OrionCommunication::DevicedBasedInfo::TV) {
			sreg.interFunc = speedRegulateDay;
			head = 0;
		} else {
			head = 2;
			sreg.interFunc = speedRegulateThermal;
		}
		sreg.zoomHead = ptzp->getHead(head);
		ptzp->setSpeedRegulation(sreg);
#endif
	}

	return Status::OK;
}

grpc::Status SmartStreamer::SetItemProperty(grpc::ServerContext *context, const OrionCommunication::ItemProperty *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context);
	qDebug() << "command is " << request->itemtype() << ", " << request->state();
	if (request->itemtype() == OrionCommunication::ItemProperty::Washer)
	{
		if (request->state())
		{
			gpiocont->setGpio(GPIO_WATER,1);
			return Status::OK;
		}
		else if (!request->state())
		{
			gpiocont->setGpio(GPIO_WATER,0);
			return Status::OK;
		}
	} else if (request->itemtype() == OrionCommunication::ItemProperty::Wiper)
	{
		if (request->state())
		{
			gpiocont->setGpio(GPIO_WIPER,1);
			return Status::OK;
		}
		else if(!request->state())
		{
			gpiocont->setGpio(GPIO_WIPER,0);
			return Status::OK;
		}
	}
	return Status::CANCELLED;
}

grpc::Status SmartStreamer::GetItemProperty(grpc::ServerContext *context, const OrionCommunication::ItemProperty *request, OrionCommunication::ItemProperty *response)
{
	Q_UNUSED(context);
	if (request->itemtype() == OrionCommunication::ItemProperty::Washer)
		response->set_state(gpiocont->getGpioValue(GPIO_WATER));
	else if (request->itemtype() == OrionCommunication::ItemProperty::Wiper)
		response->set_state(gpiocont->getGpioValue(GPIO_WIPER));
	return Status::CANCELLED;
}

grpc::Status SmartStreamer::SetBitrate(grpc::ServerContext *context, const OrionCommunication::BitrateValue *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context);
	int bitrate = request->bitratevalue();
	int brt = (bitrate / 65536);
	int brd = (bitrate % 65536);
	QJsonArray arr = sets["encoders"].toArray();
	QJsonObject arr0 = arr.at(0).toObject();
	QJsonObject arr1 = arr.at(1).toObject();
	if (brt < 1000)
		brt = 1234;
	if (brt > 10000)
		brt = 9999;
	if (brd < 1000)
		brd = 1234;
	if (brd > 10000)
		brd = 9999;
	arr0["bitrate"] = brt * 1000;
	arr1["bitrate"] = brd * 1000;
	arr = QJsonArray();
	arr << arr0;
	arr << arr1;
	sets["encoders"] = arr;
	saveSettings("settings.json", sets);
	ffDebug() << sets;
	reboot(1);
	return Status::OK;
}

grpc::Status SmartStreamer::GetBitrate(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::BitrateValue *response)
{
	Q_UNUSED(context);
	int brt = sets["encoders"].toArray().at(0).toObject()["bitrate"].toInt() / 1000;
	int brd = sets["encoders"].toArray().at(1).toObject()["bitrate"].toInt() / 1000;
	ffDebug() << brt << brd;
	response->set_bitratevalue(brt * 65536 + brd);
	//response->set_bitratevalue(brd);
	return Status::OK;
}

grpc::Status SmartStreamer::RunCIT(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, ::grpc::ServerWriter<OrionCommunication::CitMessage> *writer)
{
	Q_UNUSED(context);
	int state = 0;
	int sleepint = 10 * 1000;
	int currentProgress = 0;
	int progressTotal = 20;
	QStringList messages;
	messages << QString("Gunduz kamera haberlesme testi - %1 / %2");
	messages << QString("Pan-tilt kamera haberlesme testi - %1 / %2");
	messages << QString("Termal kamera haberlesme testi - %1 / %2");
	messages << QString("Gunduz kamera goruntu testi - %1 / %2");
	messages << QString("Termal kamera goruntu testi - %1 / %2");
	while (1) {
		usleep(sleepint);
		if (currentProgress++ >= progressTotal) {
			currentProgress = 0;
			state++;
			if (state > 4)
				break;
		}
		QString mes = messages[state].arg(currentProgress).arg(progressTotal);
		ffDebug() << state << currentProgress << progressTotal << mes;
		OrionCommunication::CitMessage citm;
		citm.set_currentstepprogress(currentProgress);
		citm.set_currentstepprogresstotal(progressTotal);
		citm.set_stepcount(5);
		citm.set_currentstep(state);
		citm.set_msg(mes.toStdString());
		//panFrames.set_valid(true);
		//panFrames.set_progress((currentProgress + (state * progressTotal)) / (5 * progressTotal));
		//panFrames.set_framedata(mes.toStdString().data());
		writer->Write(citm);
	}
	OrionCommunication::CitMessage citm;
	citm.set_currentstep(currentProgress);
	citm.set_currentstepprogresstotal(progressTotal);
	citm.set_stepcount(5);
	citm.set_currentstep(state);
	//citm.set_msg(messages[0].toStdString());
#if 0
	citm.set_citresult(0, QString("%1,%2,%3,%4,%5")
					   .arg(ptzp->getHead(0)->communicationElapsed() < 1000 ? "1" : "0")
					   .arg(ptzp->getHead(1)->communicationElapsed() < 1000 ? "1" : "0")
					   .arg(ptzp->getHead(2)->communicationElapsed() < 1000 ? "1" : "0")
					   .arg(dayPipelineElapsed.elapsed() < 1000 ? "1" : "0")
					   .arg(thPipelineElapsed.elapsed() < 1000 ? "1" : "0").toStdString().data());
#else
	citm.add_citresult();
	citm.add_citresult();
	citm.add_citresult();
	citm.add_citresult();
	citm.add_citresult();
	citm.set_citresult(0, QString("%1").arg(ptzp->getHead(0)->communicationElapsed() < 1000 ? "1" : "0").toStdString());
	citm.set_citresult(1, QString("%1").arg(ptzp->getHead(1)->communicationElapsed() < 1000 ? "1" : "0").toStdString());
	citm.set_citresult(2, QString("%1").arg(ptzp->getHead(2)->communicationElapsed() < 1000 ? "1" : "0").toStdString());
	citm.set_citresult(3, QString("%1").arg(dayPipelineElapsed.elapsed() < 1000 ? "1" : "0").toStdString());
	citm.set_citresult(4, QString("%1").arg(thPipelineElapsed.elapsed() < 1000 ? "1" : "0").toStdString());
#endif
	//panFrames.set_valid(true);
	//panFrames.set_progress((currentProgress + (state * progressTotal)) / (5 * progressTotal));
	//panFrames.set_framedata();
	writer->Write(citm);
	ffDebug() << citm.citresult_size() << citm.stepcount();
	return Status::OK;
}

grpc::Status SmartStreamer::RunAutoTrack(grpc::ServerContext *context, const OrionCommunication::AutoTrackQ *request, OrionCommunication::AppCommandResult *response)
{
	qDebug() << "Run auto track is invoked";
	QMutexLocker ml(&viaLock);
	Q_UNUSED(context)
	if (!QFile::exists("track_alg_parameters.txt") || !QFile::exists("track_parameters.txt")) {
		mDebug("The .txt files can't found. This program cancelled.");
		ffDebug() << "The .txt files can't found. This program cancelled.";
		return Status::CANCELLED;
	}
	if (!wrap) {
		mDebug("Helper ViaWrapper class uninitalized.");
		ffDebug() << "Helper ViaWrapper class uninitalized.";
		return Status::CANCELLED;
	}
	ffDebug() << "files are found";
	OrionCommunication::AutoTrackQ::DeviceType deviceType;
	OrionCommunication::AutoTrackQ::Mode mode;
	mode = request->mode();
	deviceType = request->device();
	ViaWrapper *tarwrap = NULL;
	if (deviceType == OrionCommunication::AutoTrackQ::TV) {
		tarwrap = wrapDayTv;
		if (0.02 > request->object().width() || 0.02 > request->object().height())
			return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "region too small", "region too small");
		if (0.3 < request->object().width() || 0.3 < request->object().height())
			return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "region too big", "region too big");
	} else if (deviceType == OrionCommunication::AutoTrackQ::Thermal){
		tarwrap = wrapThermal;
		if (0.02 > request->object().width() || 0.02 > request->object().height())
			return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "region too small", "region too small");
		if (0.3 < request->object().width() || 0.3 < request->object().height())
			return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "region too small", "region too small");
	} else {
		return Status::CANCELLED;
	}
	period = 0;
	tarwrap->track.score = (int)(100 * request->trackscore());
	tarwrap->track.interval = 15* request->trackinterval();
	if (mode == OrionCommunication::AutoTrackQ::DistanceToCenter) {
		tarwrap->track.mode = STrack::Mode::DistanceToCenter;
	} else if (mode == OrionCommunication::AutoTrackQ::SizeOfObject) {
		tarwrap->track.mode = STrack::Mode::SizeOfObject;
	} else if (mode == OrionCommunication::AutoTrackQ::UserMode) {
		tarwrap->track.mode = STrack::Mode::UserMode;
		tarwrap->track.objWidth = request->object().width();
		tarwrap->track.objHeight = request->object().height();
		tarwrap->track.objPointx = request->object().point_x();
		tarwrap->track.objPointy = request->object().point_y();
		ffDebug() << "!!!!!!!!!!!!!!!!!!!!!!!" << tarwrap->track.objWidth << tarwrap->track.objHeight << tarwrap->track.objPointx << tarwrap->track.objPointy;
	} else {
		mDebug("Mode request failed");
		return Status::CANCELLED;
	}

	wrapThermal->stopStabilization();
	wrapDayTv->stopStabilization();

	tarwrap->startTrack();
	wrap = tarwrap;
	qDebug() << "startTrack is invoked";
	return Status::OK;
}

grpc::Status SmartStreamer::StopAutoTrack(grpc::ServerContext *context, const OrionCommunication::DummyInfo *request, OrionCommunication::AppCommandResult *response)
{
	Q_UNUSED(context)
	QMutexLocker ml(&viaLock);
	if (wrap->track.start == true) {
		wrap->stopTrack();
		pt->panTiltStop();
		//enableVideoStabilization = true;
		//wrapThermal->startStabilization();
		//wrapDayTv->startStabilization();
	}
	return Status::OK;
}

void SmartStreamer::timeout()
{
	//ffDebug() <<  ptzp->getHead(1)->getPanAngle() << " , " << ptzp->getHead(1)->getTiltAngle();
	//ffDebug() << pt->panTiltGoPos(0,0);
	//	if (PRINT_CUDA && wrap) {
//		ffDebug() << "Motion State: " << "Stop state = " <<  wrap->motion.stop  << "Start state = " << wrap->motion.start;
//		ffDebug() << "Panaroma state: " << "Stop state = " <<  wrap->panaroma.stop  << "Start state = " << wrap->panaroma.start;
//	}
//	if (!ptzpStatus)
//		ffDebug() << "PTZP driver: Connection state halted" << pars.ptzUrl;
#if 0
	qDebug() << "CIIIIIIIIIIIIIIIIIIITTTTT"
			 << ptzp->getHead(0)->communicationElapsed()
				<< ptzp->getHead(1)->communicationElapsed()
				   << ptzp->getHead(2)->communicationElapsed()
					  << dayPipelineElapsed.elapsed()
						 << thPipelineElapsed.elapsed();
#endif

	PipelineManager::timeout();
}

void SmartStreamer::addRtspServer(RtpTransmitter *rtpout, RtpTransmitter *rtpoutvs, int sno)
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
	printPar(ptzUrl);
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
