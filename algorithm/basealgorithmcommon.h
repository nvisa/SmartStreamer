#ifndef BASEALGORITHMCOMMON_H
#define BASEALGORITHMCOMMON_H

#include <QString>
#include <QJsonObject>
#include "proto/AlgorithmWorks.grpc.pb.h"

class BaseAlgorithmCommon
{
public:
	static BaseAlgorithmCommon* instance()
	{
		static BaseAlgorithmCommon* inst = new BaseAlgorithmCommon();
		if (inst == 0)
			inst = new BaseAlgorithmCommon();
		return inst;
	}

	struct BaseVariables {
		int rgb;
		int ill;
		int debug;
		int shadow;
		int record;
		int privacy;
		int stabilization;
	};

	static BaseVariables getAlgoParameters();
	static int getSensitivity(const QString &objName);
	static bool getMotionClassification();
	static int saveRoiPoints(aw::RoiQ troi);
	static int setSensitivity(const QString objName, int sensitivity);
protected:
	BaseAlgorithmCommon();
};

#endif // BASEALGORITHMCOMMON_H
