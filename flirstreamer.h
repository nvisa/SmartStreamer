#ifndef FLIRSTREAMER_H
#define FLIRSTREAMER_H

#include "tk1streamer.h"

class TK1OmxPipeline;
class FlirStreamer: public TK1Streamer
{
	Q_OBJECT
public:
	explicit FlirStreamer(const QJsonObject &config, QObject *parent = 0);
	BaseLmmPipeline* generatePipeline();
protected:
	int pipelineOutput(BaseLmmPipeline *p, const RawBuffer &buf);
private:
	TK1OmxPipeline *dec;
};

#endif // FLIRSTREAMER_H
