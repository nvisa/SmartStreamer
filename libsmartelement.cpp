#include "libsmartelement.h"
#include "inc/BasicStructures.h"

#include <lmm/debug.h>
#include <lmm/tools/unittimestat.h>

#include <QThread>

using namespace aselsmart;

LibSmartElement * LibSmartElement::inst = nullptr;

/*
 * We have following mappings to/from libsmart strcutures:
 *
 * aselsmart::BBox <-> DetectedObject
 * ??? <-> Rectangle
 * aselsmart::MotionDirection <-> MotionDirection
 * aselsmart::points <-> Point
 * aselsmart::ROI <-> SmartMotionRegion
 *
 */

static void toGrpc(const BBox &box, algorithm::v2::DetectedObject &obj, int w, int h)
{
	auto rect = obj.mutable_bounding_box();
	auto pt = rect->mutable_top_left();
	pt->set_x(box.uS / (float)w);
	pt->set_y(box.vS / (float)h);
	pt = rect->mutable_bottom_right();
	pt->set_x(box.uE / (float)w);
	pt->set_y(box.vE / (float)h);
}

static void toGrpc(const ROI &roi, algorithm::v2::SmartMotionRegion &region, int w, int h)
{
	//roi.list_of_points
	for (auto p : roi.list_of_points) {
		auto pt = region.add_detection_region_point();
		pt->set_x(p.u / (float)w);
		pt->set_y(p.v / (float)h);
	}
	region.set_active(roi.active);
	/* TODO: convert to frame times */
	region.set_stand_time_msecs(roi.stand_time);
	region.set_wander_time_msecs(roi.wander_time);
	for (auto d: roi.valid_directions_for_alarm) {
		region.add_motion_direction(
					algorithm::v2::MotionDirection(d));
	}
}

static void toGrpc(const LINE &line, algorithm::v2::LineCrossRegion &region, int w, int h)
{
	auto pt1 = line.list_of_points[0];
	auto pt2 = line.list_of_points[1];
	region.mutable_pt1()->set_x(pt1.u / float(w));
	region.mutable_pt1()->set_y(pt1.v / float(h));
	region.mutable_pt2()->set_x(pt2.u / float(w));
	region.mutable_pt2()->set_y(pt2.v / float(h));
	region.set_active(true);
}

class LibSmartContext
{
public:
	class InitThread : public QThread
	{
	public:
		LibSmartContext *ctx;
		void run()
		{
			ctx->algoCtx = init_algorithm(ALGORITHM_SMART_MOTION, ctx->input, ctx->vbuf);
		}
	};

	LibSmartContext(const QJsonObject &conf)
	{
		config = conf;
		input = nullptr;
		output = nullptr;
		algoCtx = nullptr;
		vbuf = nullptr;
		thr = nullptr;
		settings.set_tamper_detection(config["tamper"].toBool());
		settings.set_video_stabilization(config["stabilization"].toBool());
		settings.set_privacy_masking(config["privacy"].toBool());
		settings.set_run_mode(algorithm::v2::AlgorithmParameters_RunMode_SMART_MOTION);
		settings.mutable_smart_parameters();
	}

	void setSettings(const algorithm::v2::AlgorithmParameters &sets)
	{
		settings = sets;
	}

	void setTrackRect(QRectF rect)
	{
		ffDebug() << "setting track rectangle as " << rect;
		input->ui.track_object.uS = rect.x() * 1920;
		input->ui.track_object.vS = rect.y() * 1080;
		input->ui.track_object.uE = (rect.x() + rect.width()) * 1920;
		input->ui.track_object.vE = (rect.y() + rect.height()) * 1080;
	}

	algorithm::v2::AlgorithmParameters getSettings()
	{
		return settings;
	}

	int process(const RawBuffer &buf)
	{
		if (!input)
			init();

		if (!vbuf)
			vbuf = new VideoBuffer;
		vbuf->w = buf.constPars()->videoWidth;
		vbuf->h = buf.constPars()->videoHeight;
		vbuf->pixelFormat = PIXEL_FORMAT_YUV420P;
		vbuf->buffer = (unsigned char *)buf.constData();

		if (thr) {
			if (!thr->isFinished()) {
				qDebug("Waiting init thread");
				return 0;
			}
			delete thr;
			thr = nullptr;
		}

		if (!algoCtx) {
			thr = new InitThread;
			thr->ctx = this;
			thr->start();
			return 0;
		}

		int err = processSmartMotion();
		if (err)
			return err;

		return processOutputs((RawBuffer *)&buf);
	}

protected:
	int processOutputs(RawBuffer *buf)
	{
		auto res = std::make_shared<algorithm::v2::Alarm>();
		for (size_t i = 0; i < output->alarms.size(); i++) {
			AlarmTypes alarm = output->alarms[i];
			for (size_t j = 0; j < alarm.object_BBs.size(); j++) {
				BBox box = alarm.object_BBs[j];
				auto obj = res->add_detected_object();
				toGrpc(box, *obj,
					   buf->constPars()->videoWidth,
					   buf->constPars()->videoHeight);
			}
		}
		buf->pars()->metaDataRaw = res;
		return 0;
	}

	int processSmartMotion()
	{
		std::vector<VideoBuffer *> bufs;
		bufs.push_back(vbuf);
		QElapsedTimer t;
		t.start();
		input->run_mode = (aselsmart::AlgorithmRunMode)settings.run_mode();
		input->states.stabilization = settings.video_stabilization();
		input->states.enhancement_type = settings.pre_processing();
		input->states.enhancement_degree = settings.pre_processing_degree() * 100;
		qDebug() << input->states.enhancement_type << input->states.enhancement_degree;
		stat.startStat();
		process_algorithm(algoCtx, input, output, bufs);
		stat.addStat(t.elapsed());
		//qDebug() << "***********************************" << stat.max << stat.min << stat.avg << stat.last;

		return 0;
	}

	void init()
	{
		input = new Allinputs;
		output = new Alloutputs;

		input->ui.rois = initROI();
		input->ui.lines = initLines();

		input->ui.track_object.uS = 0;
		input->ui.track_object.vS = 0;
		input->ui.track_object.uE = 100;
		input->ui.track_object.vE = 100;

		AlgorithmStates states;
		states.track = 1;
		states.videoformat = PIXEL_FORMAT_YUV420P;
		states.shadow = 1;
		states.illumination_correction = 1;
		states.debug = 1;
		states.stabilization = 0;
		states.privacy = 0;
		states.sensitivity = 95;
		states.classify = 0;
		states.enhancement_type = 2;
		states.enhancement_degree = 50;
		states.pan_tilt_zoom_read[0] = 0;
		states.pan_tilt_zoom_read[1] = 0;
		states.pan_tilt_zoom_read[2] = 0;
		states.pan_tilt_zoom_read[3] = 12;
		states.pan_tilt_zoom_read[4] = 12;

		QJsonObject track = config["track"].toObject();
		if (!track.isEmpty()) {
			states.track_states.centerize_time = track["centerize_time"].toDouble();
			states.track_states.gate_percentage = track["gate_percentage"].toDouble();
			states.track_states.scoreThreshold = track["scoreThreshold"].toDouble();
			states.track_states.track_type = track["track_type"].toInt();
		}

		input->states = states;
	}

	points createPoint(float u, float v)
	{
		points pt;
		pt.u = u;
		pt.v = v;
		return pt;
	}

	std::vector<ROI> initROI()
	{
		ROI r;
		r.active = true;
		r.wander_time = 500;
		r.stand_time = 400;
		r.list_of_points.push_back(createPoint(50, 50));
		r.list_of_points.push_back(createPoint(1600, 50));
		r.list_of_points.push_back(createPoint(1900, 1000));
		r.list_of_points.push_back(createPoint(200, 1000));
		r.valid_directions_for_alarm.push_back(DOWN_LEFT);
		r.valid_directions_for_alarm.push_back(DOWN);
		r.valid_directions_for_alarm.push_back(DOWN_RIGHT);

		r.valid_directions_for_alarm.push_back(UP);
		r.valid_directions_for_alarm.push_back(UP_RIGHT);
		r.valid_directions_for_alarm.push_back(RIGHT);
		r.valid_directions_for_alarm.push_back(LEFT);
		r.valid_directions_for_alarm.push_back(UP_LEFT);
		std::vector<ROI> l;
		l.push_back(r);

		if (settings.has_smart_parameters()) {
			auto pars = settings.mutable_smart_parameters();
			for (auto roi : l) {
				//algorithm::v2::SmartMotionRegion region;
				auto region = pars->add_regions();
				toGrpc(roi, *region, 1920, 1080);
			}
		}

		return l;
	}

	std::vector<LINE> initLines()
	{
		LINE l;
		l.active = true;
		points pt0;
		pt0.u = 1000;
		pt0.v = 700;
		points pt1;
		pt1.u = 550;
		pt1.v = 325;
		l.list_of_points.push_back(pt0);
		l.list_of_points.push_back(pt1);
		std::vector<LINE> list;
		list.push_back(l);

		if (settings.has_smart_parameters()) {
			auto pars = settings.mutable_smart_parameters();
			for (auto line: list) {
				auto region = pars->add_lines();
				toGrpc(line, *region, 1920, 1080);
			}
		}

		return list;
	}

	Allinputs *input;
	Alloutputs *output;
	VideoBuffer *vbuf;
	void *algoCtx;
	InitThread *thr;
	UnitTimeStat stat;
	algorithm::v2::AlgorithmParameters settings;
	QJsonObject config;
};

LibSmartElement *LibSmartElement::instance()
{
	return inst;
}

LibSmartElement::LibSmartElement(QObject *parent, QJsonObject config)
	: BaseLmmElement(parent)
{
	ctx = new LibSmartContext(config);
	inst = this;
}

void LibSmartElement::setVideoStabilizationEnabled(bool en)
{
	auto sets = ctx->getSettings();
	sets.set_video_stabilization(en);
	ctx->setSettings(sets);
}

bool LibSmartElement::isVideoStabilizationEnabled()
{
	return ctx->getSettings().video_stabilization();
}

bool LibSmartElement::isTamperDetectionEnabled()
{
	return ctx->getSettings().tamper_detection();
}

bool LibSmartElement::isPrivacyMaskingEnabled()
{
	return ctx->getSettings().privacy_masking();
}

algorithm::v2::AlgorithmParameters_RunMode LibSmartElement::runMode()
{
	return ctx->getSettings().run_mode();
}

int LibSmartElement::setRunMode(algorithm::v2::AlgorithmParameters_RunMode mode)
{
	auto sets = ctx->getSettings();
	sets.set_run_mode(mode);
	ctx->setSettings(sets);
	return 0;
}

void LibSmartElement::setTrackRegion(QRectF rect)
{
	ctx->setTrackRect(rect);
}

algorithm::v2::AlgorithmParameters LibSmartElement::getParameters()
{
	return ctx->getSettings();
}

void LibSmartElement::setPreprocessingType(algorithm::v2::AlgorithmParameters_PreProcessing type, float degree)
{
	auto sets = ctx->getSettings();
	sets.set_pre_processing(type);
	sets.set_pre_processing_degree(degree);
	ctx->setSettings(sets);
}

int LibSmartElement::processBuffer(const RawBuffer &buf)
{
	int err = ctx->process(buf);
	if (err)
		return err;
	return newOutputBuffer(buf);
}
