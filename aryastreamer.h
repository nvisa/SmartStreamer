#ifndef ARYASTREAMER_H
#define ARYASTREAMER_H

#include "tk1streamer.h"

class AryaStreamer: public TK1Streamer
{
	Q_OBJECT
public:
	explicit AryaStreamer(const QJsonObject &config, QObject *parent = 0);
	BaseLmmPipeline *generatePipeline();
protected:
	int pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf);
};

#endif // ARYASTREAMER_H
