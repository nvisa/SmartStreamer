#include "fusionstreamer.h"

#include <lmm/debug.h>
#include <lmm/v4l2input.h>
#include <lmm/bufferqueue.h>
#include <lmm/videoscaler.h>
#include <lmm/qtvideooutput.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/multibuffersource.h>
#include <lmm/tx1/tx1jpegencoder.h>
#include <lmm/tx1/tx1videoencoder.h>
#include <lmm/pipeline/functionpipeelement.h>

extern "C" {
#include <libavformat/avformat.h>
}

#include <opencv/cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/highgui.h>

using namespace cv;

#include <ecl/ptzp/ptzpdriver.h>
#include <ecl/ptzp/ptzphead.h>
#include <ecl/ptzp/mgeofalconeyehead.h>

#include <applicationinfo.h>

#include <QSize>

struct FovRatioStruct {
	float width_ratio = 1.0;
	float height_ratio = 1.0;
};

QMap<QString,cropRegions> cropRegionMap;

static FusionStreamer::StreamMode _mode;

FusionStreamer::FusionStreamer(const QJsonObject &config, QObject *parent)
	: TX1Streamer(parent)
{
	if (!config.empty()) {
		if (!config["device"].toString().isNull())
			sourceList =  config["device"].toString().split(";");
		else
			sourceList << "/dev/video0" << "/dev/video1";
		if (!config["resolution_width"].toString().isNull())
			width = config["resolution_width"].toInt();
		else
			width = 720;
		if (!config["resolution_height"].toString().isNull())
			height = config["resolution_height"].toInt();
		else
			height = 576;
	}
	mode = FUSION;
	_mode = mode;
	objectJson = config;
	checkCropRegion(config);
}

void FusionStreamer::setMode(FusionStreamer::StreamMode m)
{
	mode = m;
	_mode = mode;
}

void FusionStreamer::setMode(QString mode)
{
	if (mode == "day")
		setMode(DAY);
	if (mode == "thermal")
		setMode(THERMAL);
	if (mode == "fusion")
		setMode(FUSION);
}

QString FusionStreamer::getMode()
{
	if (mode == DAY)
		return "day";
	if (mode == THERMAL)
		return "thermal";
	return "fusion";
}

void FusionStreamer::checkCropRegion(const QJsonObject &config){
	if (config["fov_wide"].isObject()) {
		cropRegions region;
		QJsonObject obj = config["fov_wide"].toObject();
		region.day_strecth_x_left = obj["day_strecth_x_left"].toInt();
		region.day_strecth_y_up = obj["day_strecth_y_up"].toInt();
		region.day_strecth_x_right = obj["day_strecth_x_right"].toInt();
		region.day_strecth_y_down = obj["day_strecth_y_down"].toInt();
		region.thermal_strecth_x_left = obj["thermal_strecth_x_left"].toInt();
		region.thermal_strecth_y_up = obj["thermal_strecth_y_up"].toInt();
		region.thermal_strecth_x_right = obj["thermal_strecth_x_right"].toInt();
		region.thermal_strecth_y_down = obj["thermal_strecth_y_down"].toInt();
		cropRegionMap.insert("fov_wide",region);
	} if (config["fov_wide_middle"].isObject()) {
		cropRegions region;
		QJsonObject obj = config["fov_wide_middle"].toObject();
		region.day_strecth_x_left = obj["day_strecth_x_left"].toInt();
		region.day_strecth_y_up = obj["day_strecth_y_up"].toInt();
		region.day_strecth_x_right = obj["day_strecth_x_right"].toInt();
		region.day_strecth_y_down = obj["day_strecth_y_down"].toInt();
		region.thermal_strecth_x_left = obj["thermal_strecth_x_left"].toInt();
		region.thermal_strecth_y_up = obj["thermal_strecth_y_up"].toInt();
		region.thermal_strecth_x_right = obj["thermal_strecth_x_right"].toInt();
		region.thermal_strecth_y_down = obj["thermal_strecth_y_down"].toInt();
		cropRegionMap.insert("fov_wide_middle",region);
	} if (config["fov_middle"].isObject()) {
		cropRegions region;
		QJsonObject obj = config["fov_middle"].toObject();
		region.day_strecth_x_left = obj["day_strecth_x_left"].toInt();
		region.day_strecth_y_up = obj["day_strecth_y_up"].toInt();
		region.day_strecth_x_right = obj["day_strecth_x_right"].toInt();
		region.day_strecth_y_down = obj["day_strecth_y_down"].toInt();
		region.thermal_strecth_x_left = obj["thermal_strecth_x_left"].toInt();
		region.thermal_strecth_y_up = obj["thermal_strecth_y_up"].toInt();
		region.thermal_strecth_x_right = obj["thermal_strecth_x_right"].toInt();
		region.thermal_strecth_y_down = obj["thermal_strecth_y_down"].toInt();
		cropRegionMap.insert("fov_middle",region);
	} if (config["fov_narrow_middle"].isObject()) {
		cropRegions region;
		QJsonObject obj = config["fov_narrow_middle"].toObject();
		region.day_strecth_x_left = obj["day_strecth_x_left"].toInt();
		region.day_strecth_y_up = obj["day_strecth_y_up"].toInt();
		region.day_strecth_x_right = obj["day_strecth_x_right"].toInt();
		region.day_strecth_y_down = obj["day_strecth_y_down"].toInt();
		region.thermal_strecth_x_left = obj["thermal_strecth_x_left"].toInt();
		region.thermal_strecth_y_up = obj["thermal_strecth_y_up"].toInt();
		region.thermal_strecth_x_right = obj["thermal_strecth_x_right"].toInt();
		region.thermal_strecth_y_down = obj["thermal_strecth_y_down"].toInt();
		cropRegionMap.insert("fov_narrow_middle",region);
	} if (config["fov_narrow"].isObject()) {
		cropRegions region;
		QJsonObject obj = config["fov_narrow"].toObject();
		region.day_strecth_x_left = obj["day_strecth_x_left"].toInt();
		region.day_strecth_y_up = obj["day_strecth_y_up"].toInt();
		region.day_strecth_x_right = obj["day_strecth_x_right"].toInt();
		region.day_strecth_y_down = obj["day_strecth_y_down"].toInt();
		region.thermal_strecth_x_left = obj["thermal_strecth_x_left"].toInt();
		region.thermal_strecth_y_up = obj["thermal_strecth_y_up"].toInt();
		region.thermal_strecth_x_right = obj["thermal_strecth_x_right"].toInt();
		region.thermal_strecth_y_down = obj["thermal_strecth_y_down"].toInt();
		cropRegionMap.insert("fov_narrow",region);
	} else {
		mInfo("No valid calibration information");
	}
}

static FovRatioStruct fovRatio()
{
	float *fovs;//ApplicationInfo::instance()->getPtzpDriver()->getHead(0)->getFovs();
	FovRatioStruct fovRatio;
	fovRatio.width_ratio = fovs[0] / fovs[2];
	fovRatio.height_ratio = fovs[1] / fovs[3];
	return fovRatio;
}

static void modifyThermal(Mat &frame) {
	int width = frame.cols;
	int height = frame.rows;
	int x = 42;
	int y = 42;
	Rect crop(x,y,width - x - 40, height - y - 48); //y - 30
	frame = frame(crop);
	resize(frame,frame,Size(width,height));
}

static void modifyDay(Mat &frame) {
	int width = frame.cols;
	int height = frame.rows;

	int x = 5;
	int y = 10;

	Rect crop(x,y, width - x - 11,height - y - 10);
	frame = frame(crop);
	resize(frame,frame,Size(width,height));
}

static void cropFrames(Mat &frame1, Mat &frame2, FovRatioStruct fRs) {
	float width = (float)frame1.cols;
	float height = (float)frame1.rows;

	float width_frame1 = width; float width_frame2 = width;
	float height_frame1 = height; float height_frame2 = height;

	if (fRs.width_ratio >= 1)
		width_frame1 = width / fRs.width_ratio;
	else
		width_frame2 = width * fRs.width_ratio;

	height_frame1 = width_frame1 / width * height;
	height_frame2 = width_frame2 / width * height;

	float expected_height_frame1 = height_frame1;
	float expected_height_frame2 = height_frame2;
	if (fRs.height_ratio >= 1)
		expected_height_frame1 = height / fRs.height_ratio;
	else
		expected_height_frame2 = height * fRs.height_ratio;

	if (expected_height_frame1 <= height_frame1)
		height_frame1 = expected_height_frame1;
	if (expected_height_frame2 <= height_frame2)
		height_frame2 = expected_height_frame2;

	width_frame1 *= (height_frame1 / height);
	width_frame2 *= (height_frame2 / height);

	Rect frame1_rect;
	Rect frame2_rect;
	int fov = ApplicationInfo::instance()->getPtzpDriver()->getHead(0)->getProperty(2);

	if (fov == 0) {
		cropRegions region = cropRegionMap["fov_wide"];
		frame1_rect = Rect(0 + region.day_strecth_x_left,0 + region.day_strecth_y_up,720 + region.day_strecth_x_right,576 + region.day_strecth_y_down);
		frame2_rect = Rect(0 + region.thermal_strecth_x_left,0 + region.thermal_strecth_y_up,720 + region.thermal_strecth_x_right ,576 + region.thermal_strecth_y_down);
	}
	else if (fov == 1) {
		cropRegions region = cropRegionMap["fov_middle"];
		frame1_rect = Rect((int)((width - width_frame1) / 2) + region.day_strecth_x_left, (int)((height - height_frame1) / 2) + region.day_strecth_y_up,
						   (int)(width_frame1 + (width - width_frame1) / 2) + region.day_strecth_x_right,(int)(height_frame1 + (height - height_frame1) / 2) + region.day_strecth_y_down);
		frame2_rect = Rect((int)((width - width_frame2) / 2) + region.thermal_strecth_x_left, (int)((height - height_frame2) / 2) + region.thermal_strecth_y_up,
						   (int)(width_frame2 + (width - width_frame2) / 2) + region.thermal_strecth_x_right,(int)(height_frame2 + (height - height_frame2) / 2) + region.thermal_strecth_y_down);
	}
	else if (fov == 2) {
		cropRegions region = cropRegionMap["fov_narrow"];
		frame1_rect = Rect((int)((width - width_frame1) / 2) + region.day_strecth_x_left, (int)((height - height_frame1) / 2) + region.day_strecth_y_up,
						   (int)(width_frame1 + (width - width_frame1) / 2) + region.day_strecth_x_right,(int)(height_frame1 + (height - height_frame1) / 2) + region.day_strecth_y_down);
		frame2_rect = Rect((int)((width - width_frame2) / 2) + region.thermal_strecth_x_left, (int)((height - height_frame2) / 2) + region.thermal_strecth_y_up,
						   (int)(width_frame2 + (width - width_frame2) / 2) + region.thermal_strecth_x_right,(int)(height_frame2 + (height - height_frame2) / 2)+ region.thermal_strecth_y_down);
	}	else if (fov == 3) {
		cropRegions region = cropRegionMap["fov_wide_middle"];
		frame1_rect = Rect((int)((width - width_frame1) / 2) + region.day_strecth_x_left, (int)((height - height_frame1) / 2) + region.day_strecth_y_up,
						   (int)(width_frame1 + (width - width_frame1) / 2) + region.day_strecth_x_right,(int)(height_frame1 + (height - height_frame1) / 2) + region.day_strecth_y_down);
		frame2_rect = Rect((int)((width - width_frame2) / 2) + region.thermal_strecth_x_left, (int)((height - height_frame2) / 2) + region.thermal_strecth_y_up,
						   (int)(width_frame2 + (width - width_frame2) / 2) + region.thermal_strecth_x_right,(int)(height_frame2 + (height - height_frame2) / 2)+ region.thermal_strecth_y_down);
	} 	else if (fov == 4) {
		cropRegions region = cropRegionMap["fov_narrow_middle"];
		frame1_rect = Rect((int)((width - width_frame1) / 2) + region.day_strecth_x_left, (int)((height - height_frame1) / 2) + region.day_strecth_y_up,
						   (int)(width_frame1 + (width - width_frame1) / 2) + region.day_strecth_x_right,(int)(height_frame1 + (height - height_frame1) / 2) + region.day_strecth_y_down);
		frame2_rect = Rect((int)((width - width_frame2) / 2) + region.thermal_strecth_x_left, (int)((height - height_frame2) / 2) + region.thermal_strecth_y_up,
						   (int)(width_frame2 + (width - width_frame2) / 2) + region.thermal_strecth_x_right,(int)(height_frame2 + (height - height_frame2) / 2)+ region.thermal_strecth_y_down);
	} else {
		frame1_rect = Rect(0,0,720,576);
		frame2_rect = Rect(0,0,720,576);
	}
	frame1 = frame1(frame1_rect);
	frame2 = frame2(frame2_rect);
}

static void resizeFrames(Mat &frame1, Mat &frame2, Size reference) {
	resize(frame1, frame1, reference);
	resize(frame2, frame2, reference);
}

static void doWeightedAddForYUV(Mat &frame1, Mat &frame2 , Mat &frame3, Mat &frame4, uchar *outdata) {
	int width = frame1.cols;
	int height = frame1.rows;
	addWeighted(frame1,0.6,frame4,0.4,0,frame1);
	memcpy(outdata, frame1.data, width * height);
	memcpy(outdata + width * height, frame2.data, width * height / 4);
	memcpy(outdata + width * height * 5 / 4, frame3.data, width * height / 4);
}

static RawBuffer fuseStreamsYUV(const RawBuffer &buf)
{
	if (buf.getMimeType() != "application/multi-buffer")
		return RawBuffer();

	if (_mode == FusionStreamer::THERMAL) { // Switching to only thermal, info will be obtained from ptzp using ApplicationInfo
		return buf.constPars()->subbufs[0];
	} else if (_mode == FusionStreamer::DAY) { // Switching to only day, info will be obtained from ptzp using ApplicationInfo
		return buf.constPars()->subbufs[1];
	}

	RawBuffer ch0 = buf.constPars()->subbufs[0];
	RawBuffer ch1 = buf.constPars()->subbufs[1];
	int width = ch0.constPars()->videoWidth;
	int height = ch0.constPars()->videoHeight;

	unsigned char *buf_ch0 = (uchar *)ch0.constData();
	unsigned char *buf_ch1 = (uchar *)ch1.constData();

	Mat frameThermalY(height, width, CV_8U, (void *)buf_ch0);
	Mat frameDayY(height, width, CV_8U, (void *)buf_ch1);
	Mat frameDayU(height / 2, width / 2, CV_8U, (void *)buf_ch1+width*height);
	Mat frameDayV(height / 2, width / 2, CV_8U, (void *)buf_ch1+width*height+(width * height / 4));

	modifyThermal(frameThermalY);
	//modifyDay(frameDayY);

	FovRatioStruct fovRatioForCrop = fovRatio();
	cropFrames(frameDayY,frameThermalY,fovRatioForCrop);

	Size sizeOfFrame(width,height);
	resizeFrames(frameDayY, frameThermalY, sizeOfFrame);

	RawBuffer outBuf("video/x-raw-yuv",width * height * 3 / 2);
	uchar *dst = (uchar *)outBuf.data();
	doWeightedAddForYUV(frameDayY, frameDayU, frameDayV, frameThermalY, dst);

	outBuf.pars()->videoWidth = width;
	outBuf.pars()->videoHeight = height;
	outBuf.pars()->avPixelFormat = AV_PIX_FMT_YUV420P;
	outBuf.pars()->captureTime = ch0.constPars()->captureTime;
	outBuf.pars()->streamBufferNo = ch0.constPars()->streamBufferNo;

	return outBuf;
}

BaseLmmPipeline* FusionStreamer::createYUV420Pipeline(QSize &res0)
{
	res0.setWidth(width);
	res0.setHeight(height);
	MultiBufferSource *multisrc = new MultiBufferSource;

	for (int i = 0; i < sourceList.size(); i++) {
		V4l2Input *v4l2 = new V4l2Input;
		v4l2->setParameter("videoWidth",width);
		v4l2->setParameter("videoHeight",height);
		v4l2->setParameter("device",sourceList[i]);

		BaseLmmPipeline *p = addPipeline();
		p->append(v4l2);

		VideoScaler *toYuv = new VideoScaler;
		toYuv->setOutputFormat(AV_PIX_FMT_YUV420P);
		toYuv->setMode(1);

		p->append(toYuv);
		p->append(multisrc, i);
		p->end();
	}

	BaseLmmPipeline *p = addPipeline();
	p->append(multisrc, -1);
	p->append(new FunctionPipeElement(&fuseStreamsYUV));

	return p;
}
