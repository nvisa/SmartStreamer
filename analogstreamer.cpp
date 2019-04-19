#include "analogstreamer.h"

#include <lmm/debug.h>
#include <lmm/v4l2input.h>
#include <lmm/videoscaler.h>
#include <lmm/qtvideooutput.h>
#include <lmm/baselmmpipeline.h>
#include <lmm/multibuffersource.h>
#include <lmm/pipeline/functionpipeelement.h>

extern "C" {
#include <libavformat/avformat.h>
}

AnalogStreamer::AnalogStreamer(const QJsonObject &config, QObject *parent)
	: BaseStreamer(parent)
{
	V4l2Input *v4l2 = new V4l2Input;
	v4l2->setParameter("videoWidth", 720);
	v4l2->setParameter("videoHeight", 576);
	if (config.isEmpty())
		v4l2->setParameter("device", QString("/dev/video0"));
	else
		v4l2->setParameter("device", QString(config["device"].toString()));

	VideoScaler *toRgb = new VideoScaler;
	toRgb->setOutputFormat(AV_PIX_FMT_ARGB);
	toRgb->setMode(1);

	BaseLmmPipeline *p = addPipeline();
	p->append(v4l2);
	p->append(toRgb);
	p->append(new QtVideoOutput);
	p->end();
}

