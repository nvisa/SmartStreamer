#ifndef PANAROMAALGORITHMELEMENT_H
#define PANAROMAALGORITHMELEMENT_H

#include "basealgorithmelement.h"
#include "ecl/ptzp/ptzphead.h"
#include "ecl/ptzp/aryapthead.h"

#include <QElapsedTimer>

class PanaromaAlgorithmElement : public BaseAlgorithmElement
{
	Q_OBJECT
public:
	explicit PanaromaAlgorithmElement(QObject *parent = 0);
	int init();
	int reinit();
	int processAlgo(const RawBuffer &buf);
	int reallocate();
	int stopAlgo();
	int release();
	struct PanaromaControl {
		uchar meta[4096];
		struct Started {
			int pan;
			int tilt;
		};
		Started started;
		float panTiltZoomRead[10];
		int init;
	};
	PanaromaControl getPanaromaControl() { return control;}
	QByteArray getPanaromaFrame(const QString &picture);
	virtual QString getTypeString();
protected:
	PtzpHead* getPanTiltHead();
	int resetPosition();
	int doPivot(float speed);
	int reloadJson(const QJsonObject &node);
	QJsonObject resaveJson(const QJsonObject &node);
private:
	PanaromaControl control;
	PtzpHead *pt;
};

#endif // PANAROMAALGORITHMELEMENT_H
