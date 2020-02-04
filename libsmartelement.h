#ifndef LIBSMARTELEMENT_H
#define LIBSMARTELEMENT_H

#include <lmm/baselmmelement.h>

#include "algorithmcommunicationv2.h"
#include "proto/v2/AlgorithmCommunicationV2.pb.h"

#include <QRectF>
#include <QJsonObject>

class LibSmartContext;

class LibSmartElement : public BaseLmmElement
{
	Q_OBJECT
public:
	static LibSmartElement * instance();
	LibSmartElement(QObject *parent = NULL, QJsonObject config = QJsonObject());

	void setVideoStabilizationEnabled(bool en);
	bool isVideoStabilizationEnabled();
	bool isTamperDetectionEnabled();
	bool isPrivacyMaskingEnabled();
	algorithm::v2::AlgorithmParameters_RunMode runMode();
	int setRunMode(algorithm::v2::AlgorithmParameters_RunMode mode);
	void setTrackRegion(QRectF rect);
	algorithm::v2::AlgorithmParameters getParameters();
	void setPreprocessingType(algorithm::v2::AlgorithmParameters_PreProcessing type, float degree);

	int processBuffer(const RawBuffer &buf);

protected:
	LibSmartContext *ctx;
	static LibSmartElement *inst;
};

#endif // LIBSMARTELEMENT_H
