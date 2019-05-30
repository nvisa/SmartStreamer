#include "stabilizationalgorithmelement.h"
#include "algorithmfunctions.h"
#include "applicationinfo.h"

#include <lmm/debug.h>

#include <ecl/ptzp/ptzphead.h>
#include <ecl/ptzp/ptzpdriver.h>

StabilizationAlgorithmElement::StabilizationAlgorithmElement(QObject *parent)
	: BaseAlgorithmElement(parent)
{
	//printf("start zoom reading\n");
	ZoomLevelNo = 258;
	FILE *file_alg_params = fopen("/etc/smartstreamer/Zoom_value.txt", "r");
	for(int ii=0;ii<ZoomLevelNo;ii++)
			fscanf(file_alg_params, "%f%*[^\n]",&zoomvalues[ii]);
	fclose(file_alg_params);

	FILE *file_alg_params2 = fopen("/etc/smartstreamer/Hfov.txt", "r");
	for(int ii=0;ii<ZoomLevelNo;ii++)
			fscanf(file_alg_params2, "%f%*[^\n]",&zoomvalues[ii + ZoomLevelNo]);
	fclose(file_alg_params2);

	FILE *file_alg_params3 = fopen("/etc/smartstreamer/Vfov.txt", "r");
	for(int ii=0;ii<ZoomLevelNo;ii++)
			fscanf(file_alg_params3, "%f%*[^\n]",&zoomvalues[ii + 2*ZoomLevelNo]);
	fclose(file_alg_params3);

	printf("end zoom reading\n");
}
void StabilizationAlgorithmElement::zoom2degree_conversion(int zoomReadOut,float* HV_fovAngles)
{
	int index = 0;
	for(int i=0;i<ZoomLevelNo;i++)
	{
		if(zoomReadOut<=zoomvalues[i])
		{
			index = i;
			break;
		}
	}
	//printf("index: %d -- %d\n",index,zoomReadOut);
	if(index==0)//max zoom out
	{
		HV_fovAngles[3] = zoomvalues[index + ZoomLevelNo];
		HV_fovAngles[4] = zoomvalues[index + ZoomLevelNo*2];
	}
	else if(index==ZoomLevelNo-1)// max zoom in
	{
		HV_fovAngles[3] = zoomvalues[index + ZoomLevelNo];
		HV_fovAngles[4] = zoomvalues[index + ZoomLevelNo*2];
	}
	else // interpolate zoom
	{
		float w1 = zoomvalues[index] - zoomReadOut;
		float w0 = - zoomReadOut - zoomvalues[index-1];

		HV_fovAngles[3] = (w1*zoomvalues[index + ZoomLevelNo] + w0*zoomvalues[index-1 + ZoomLevelNo])/(w0+w1);
		HV_fovAngles[4] = (w1*zoomvalues[index + 2*ZoomLevelNo] + w0*zoomvalues[index-1 + 2*ZoomLevelNo])/(w0+w1);

		//printf("H-F intermadiate values: %f %f\n",zoomvalues[index + ZoomLevelNo],zoomvalues[index + 2*ZoomLevelNo]);
	}
	//printf("H-F angles: %f %f\n",HV_fovAngles[3],HV_fovAngles[4]);
}

int StabilizationAlgorithmElement::init()
{
	v.debug = 0;
	v.ill = 1;
	v.privacy = 1;
	v.record = 0;
	v.rgb = 1;
	v.shadow = 0;
	v.stabilization = 0;
	control.initialize = 1;
	v.rgb = 1;
	return BaseAlgorithmElement::init();
}

int StabilizationAlgorithmElement::reallocate()
{
	return 0;
}

int StabilizationAlgorithmElement::processAlgo(const RawBuffer &buf)
{
	int width = buf.constPars()->videoWidth;
	int height = buf.constPars()->videoHeight;
	control.sensitivity = BaseAlgorithmCommon::instance()->getSensitivity("stabilization");
	PtzpHead *headz = ApplicationInfo::instance()->getPtzpDriver(0)->getHead(0);
	PtzpHead *headpt = ApplicationInfo::instance()->getPtzpDriver(0)->getHead(1);
	float panTiltZoomRead[] = {10, 10, 0, 57.6, 34.3}; //some sane defaults
	if (headpt) {
		panTiltZoomRead[0] = headpt->getPanAngle();
		panTiltZoomRead[1] = headpt->getTiltAngle();
	}
	if (headz) {
		//zoom2degree_conversion(headz->getZoom(), panTiltZoomRead);
		float fovh = 0, fovv = 0;
		if (!headz->getFOV(fovh, fovv)) {
			panTiltZoomRead[3] = fovh;
			panTiltZoomRead[4] = fovv;
		}
	}

	asel_bypass((uchar *)buf.constData(), width * height, width, height,
				panTiltZoomRead, v.debug, v.stabilization,v.privacy,
				control.initialize, control.sensitivity, v.rgb);

	if (control.initialize)
		control.initialize = 0;

	mInfo("Stabilization algorithm processing");
	return 0;
}

int StabilizationAlgorithmElement::release()
{
	asel_bypass_release();
	return 0;
}

QString StabilizationAlgorithmElement::getTypeString()
{
	return "bypass";
}

